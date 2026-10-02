// Selected candidate-only summaries. Run readOnly/noanalysis; never modifies a program.
// Args: output.json original-binary signed-delta selectors.json
import ghidra.app.script.GhidraScript;
import ghidra.framework.Application;
import ghidra.program.model.address.*;
import ghidra.program.model.block.*;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.pcode.*;
import ghidra.program.model.symbol.FlowType;
import com.google.gson.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.channels.FileChannel;
import java.nio.file.*;
import java.security.MessageDigest;
import java.util.*;

public class ExportSelectedFunctionShapes extends GhidraScript {
    private static final int MAX_FUNCTIONS=16, MAX_INSTRUCTIONS=4096, MAX_BODY=131072;
    private static final int MAX_BLOCKS=2048, MAX_EDGES=4096, MAX_PCODE=262144;
    private static final int MAX_ROW=65536, MAX_OUTPUT=2097152;
    private final Gson json=new GsonBuilder().serializeNulls().disableHtmlEscaping().create();
    private long delta; private int totalBody,totalBlocks,totalEdges,totalPcode;
    private String binaryHash; private Register stackPointer;
    private final List<long[]> originalRanges=new ArrayList<>();
    private static Map<String,Object> rec(Object... v) {
        Map<String,Object> r=new LinkedHashMap<>();
        for(int i=0;i<v.length;i+=2)r.put((String)v[i],v[i+1]);return r;
    }
    private static void inc(Map<String,Integer> m,String k){m.put(k,m.getOrDefault(k,0)+1);}
    private static String digest(MessageDigest m){return HexFormat.of().formatHex(m.digest());}
    private String hash(Path p)throws Exception {
        MessageDigest m=MessageDigest.getInstance("SHA-256");
        try(InputStream s=Files.newInputStream(p)){byte[] b=new byte[65536];int n;while((n=s.read(b))>=0){monitor.checkCancelled();m.update(b,0,n);}}return digest(m);
    }
    private long u32(RandomAccessFile f,long p)throws Exception{f.seek(p);return Integer.toUnsignedLong(Integer.reverseBytes(f.readInt()));}
    private int u16(RandomAccessFile f,long p)throws Exception{f.seek(p);return Short.toUnsignedInt(Short.reverseBytes(f.readShort()));}
    private long imageBase(Path p)throws Exception {
        try(RandomAccessFile f=new RandomAccessFile(p.toFile(),"r")){
            long magic=u32(f,0);
            if(magic==0x464c457fL){
                f.seek(4);if(f.readUnsignedByte()!=1||f.readUnsignedByte()!=1)throw new IllegalArgumentException("Need LE ELF32");
                long table=u32(f,28),base=Long.MAX_VALUE;int stride=u16(f,42),count=u16(f,44);
                if(stride<32||count<1||count>256)throw new IllegalArgumentException("ELF header bound");
                for(int i=0;i<count;i++){long e=table+(long)i*stride;if(u32(f,e)==1){long a=u32(f,e+8),z=u32(f,e+20);if(a+z>0x100000000L)throw new IllegalArgumentException("ELF range overflow");originalRanges.add(new long[]{a,a+z});base=Math.min(base,a);}}
                if(base==Long.MAX_VALUE)throw new IllegalArgumentException("No ELF PT_LOAD");return base;
            }
            if((magic&0xffff)==0x5a4d){long e=u32(f,0x3c);if(u32(f,e)!=0x4550||u16(f,e+24)!=0x10b)throw new IllegalArgumentException("Need PE32");long a=u32(f,e+52),z=u32(f,e+80);if(a+z>0x100000000L)throw new IllegalArgumentException("PE overflow");originalRanges.add(new long[]{a,a+z});return a;}
            throw new IllegalArgumentException("Need original PE32/ELF32");
        }
    }
    private String va(Address a){long v=a.getOffset()+delta;if(v<0||v>0xffffffffL)throw new IllegalArgumentException("VA overflow");return String.format("0x%08x",v);}
    private boolean original(Address a){if(a==null||!a.isMemoryAddress()||!a.getAddressSpace().equals(currentProgram.getAddressFactory().getDefaultAddressSpace()))return false;long v=a.getOffset()+delta;for(long[] r:originalRanges)if(v>=r[0]&&v<r[1])return true;return false;}
    private List<Map<String,Object>> ranges(AddressSetView body){List<Map<String,Object>> r=new ArrayList<>();for(AddressRange a:body.getAddressRanges()){if(r.size()>=4096)throw new IllegalArgumentException("Range cap");r.add(rec("start",va(a.getMinAddress()),"end_exclusive",String.format("0x%08x",a.getMaxAddress().getOffset()+delta+1)));}return r;}
    private String vnKey(Varnode v){return v.getAddress().toString()+":"+v.getSize();}
    // Conservative within-instruction address dependency classification, not type recovery.
    private String addressClass(Varnode v,Map<String,PcodeOp> defs,Set<String> active,int depth){
        if(v==null||depth>16)return "register_or_unknown";
        if(v.isConstant())return "absolute";
        if(v.isRegister()){if(stackPointer!=null&&v.getAddress().equals(stackPointer.getAddress())&&v.getSize()==stackPointer.getMinimumByteSize())return "stack_relative";return "register_or_unknown";}
        String key=vnKey(v);if(!active.add(key))return "register_or_unknown";
        PcodeOp op=defs.get(key);String result="register_or_unknown";
        if(op!=null){int code=op.getOpcode();if(code==PcodeOp.COPY&&op.getNumInputs()==1)result=addressClass(op.getInput(0),defs,active,depth+1);
            else if((code==PcodeOp.INT_ADD||code==PcodeOp.INT_SUB||code==PcodeOp.INT_AND)&&op.getNumInputs()==2){String l=addressClass(op.getInput(0),defs,active,depth+1),r=addressClass(op.getInput(1),defs,active,depth+1);if(l.equals("absolute")&&r.equals("absolute"))result="absolute";else if(l.equals("stack_relative")&&r.equals("absolute"))result="stack_relative";else if(code==PcodeOp.INT_ADD&&l.equals("absolute")&&r.equals("stack_relative"))result="stack_relative";}}
        active.remove(key);return result;
    }
    private boolean flowPcode(int code){return code==PcodeOp.CALL||code==PcodeOp.CALLIND||code==PcodeOp.BRANCH||code==PcodeOp.BRANCHIND||code==PcodeOp.CBRANCH||code==PcodeOp.RETURN;}
    private Map<String,Integer> memoryBasis(){Map<String,Integer> m=new TreeMap<>();for(String k:Arrays.asList("LOAD_operations","STORE_operations","direct_data_read_varnodes","direct_data_write_varnodes","excluded_flow_memory_varnodes","excluded_LOAD_STORE_embedded_memory_varnodes"))m.put(k,0);return m;}
    private void accountMemory(PcodeOp[] ops,Map<String,PcodeOp> defs,Map<String,Integer> memory,Map<String,Integer> basis){
        for(PcodeOp op:ops){int code=op.getOpcode();
            if(code==PcodeOp.LOAD||code==PcodeOp.STORE){boolean load=code==PcodeOp.LOAD;int width=load?op.getOutput().getSize():op.getInput(2).getSize();String mode=addressClass(op.getInput(1),defs,new HashSet<>(),0);inc(memory,(load?"read":"write")+":"+mode+":"+width);inc(basis,load?"LOAD_operations":"STORE_operations");
                // Intrinsic access already counted. Unusual RAM-storage pointer/value operands
                // are recorded as excluded debt rather than double-counted or silently normalized.
                for(Varnode v:op.getInputs())if(v.isAddress())inc(basis,"excluded_LOAD_STORE_embedded_memory_varnodes");if(op.getOutput()!=null&&op.getOutput().isAddress())inc(basis,"excluded_LOAD_STORE_embedded_memory_varnodes");continue;}
            if(flowPcode(code)){for(Varnode v:op.getInputs())if(v.isAddress())inc(basis,"excluded_flow_memory_varnodes");if(op.getOutput()!=null&&op.getOutput().isAddress())inc(basis,"excluded_flow_memory_varnodes");continue;}
            Set<String> reads=new HashSet<>();for(Varnode v:op.getInputs())if(v.isAddress()&&reads.add(vnKey(v))){inc(memory,"read:absolute:"+v.getSize());inc(basis,"direct_data_read_varnodes");}
            Varnode out=op.getOutput();if(out!=null&&out.isAddress()){inc(memory,"write:absolute:"+out.getSize());inc(basis,"direct_data_write_varnodes");}
        }
    }
    private void require(boolean value,String message){if(!value)throw new IllegalArgumentException("Synthetic memory accounting: "+message);}
    private void memoryAccountingSelfTest(){
        AddressFactory af=currentProgram.getAddressFactory();AddressSpace ram=af.getDefaultAddressSpace(),constant=af.getConstantSpace(),unique=af.getUniqueSpace();Address site=ram.getAddress(0x1000);Varnode value=new Varnode(constant.getAddress(7),4),space=new Varnode(constant.getAddress(Integer.toUnsignedLong(ram.getSpaceID())),4),pointer=new Varnode(unique.getAddress(0x200),4);List<PcodeOp> pc=new ArrayList<>(),aa=new ArrayList<>();
        for(int k=0;k<5;k++){pc.add(new PcodeOp(site,k,k%2==0?PcodeOp.COPY:PcodeOp.FLOAT_FLOAT2FLOAT,new Varnode[]{value},new Varnode(ram.getAddress(0x3000+4*k),4)));aa.add(new PcodeOp(site,k,PcodeOp.STORE,new Varnode[]{space,pointer,value},null));}
        Map<String,Integer> m=new TreeMap<>(),basis=memoryBasis();accountMemory(pc.toArray(new PcodeOp[0]),new HashMap<>(),m,basis);require(m.getOrDefault("write:absolute:4",0)==5&&basis.get("direct_data_write_varnodes")==5&&basis.get("STORE_operations")==0,"five PC direct stores");
        m.clear();basis=memoryBasis();accountMemory(aa.toArray(new PcodeOp[0]),new HashMap<>(),m,basis);require(m.getOrDefault("write:register_or_unknown:4",0)==5&&basis.get("STORE_operations")==5&&basis.get("direct_data_write_varnodes")==0,"five AA indirect stores without double count");
        List<PcodeOp> targets=new ArrayList<>();for(int code:new int[]{PcodeOp.CALL,PcodeOp.BRANCH,PcodeOp.CBRANCH,PcodeOp.RETURN,PcodeOp.CALLIND,PcodeOp.BRANCHIND})targets.add(new PcodeOp(site,0,code,new Varnode[]{new Varnode(ram.getAddress(0x4000),4)},null));m.clear();basis=memoryBasis();accountMemory(targets.toArray(new PcodeOp[0]),new HashMap<>(),m,basis);require(m.isEmpty()&&basis.get("excluded_flow_memory_varnodes")==6,"flow address is not data access");
        Varnode word=new Varnode(ram.getAddress(0x5000),2),tmp=new Varnode(unique.getAddress(0x300),4);m.clear();basis=memoryBasis();accountMemory(new PcodeOp[]{new PcodeOp(site,0,PcodeOp.INT_ZEXT,new Varnode[]{word},tmp)},new HashMap<>(),m,basis);require(m.getOrDefault("read:absolute:2",0)==1&&basis.get("direct_data_read_varnodes")==1,"MOVZX uses source width2");
        m.clear();basis=memoryBasis();accountMemory(new PcodeOp[]{new PcodeOp(site,0,PcodeOp.INT_ADD,new Varnode[]{word,word},tmp)},new HashMap<>(),m,basis);require(m.getOrDefault("read:absolute:2",0)==1,"duplicate direct input dedup per operation");
        Varnode data=new Varnode(ram.getAddress(0x6000),4);m.clear();basis=memoryBasis();accountMemory(new PcodeOp[]{new PcodeOp(site,0,PcodeOp.INT_ADD,new Varnode[]{data,value},data)},new HashMap<>(),m,basis);require(m.getOrDefault("read:absolute:4",0)==1&&m.getOrDefault("write:absolute:4",0)==1,"read-modify-write two roles");
        Varnode absolute=new Varnode(constant.getAddress(0x7000),4);m.clear();basis=memoryBasis();accountMemory(new PcodeOp[]{new PcodeOp(site,0,PcodeOp.LOAD,new Varnode[]{space,absolute},tmp),new PcodeOp(site,1,PcodeOp.STORE,new Varnode[]{space,absolute,value},null)},new HashMap<>(),m,basis);require(m.getOrDefault("read:absolute:4",0)==1&&m.getOrDefault("write:absolute:4",0)==1&&basis.get("direct_data_read_varnodes")==0&&basis.get("direct_data_write_varnodes")==0,"LOAD STORE intrinsic basis only");
    }
    private String opcodeClass(Instruction i){String m=i.getMnemonicString().toUpperCase(Locale.ROOT);FlowType f=i.getFlowType();
        if(f.isCall())return f.isComputed()?"call_indirect":"call_direct";
        if(m.startsWith("RET")||m.startsWith("IRET"))return "return";
        if(f.isJump())return f.isComputed()?"branch_indirect":f.isConditional()?"branch_conditional":"branch_direct";
        if(m.equals("NOP"))return "nop";
        if(m.startsWith("FX")||m.equals("LDMXCSR")||m.equals("STMXCSR")||m.equals("EMMS"))return "FP_state_control";
        if(m.startsWith("F"))return "x87";
        if(m.matches(".*(SS|SD|PS|PD)$"))return "SSE_FP_or_move";
        if(Arrays.asList("CMP","TEST").contains(m))return "integer_compare";
        if(Arrays.asList("AND","OR","XOR","NOT","SHL","SHR","SAR","SAL","ROL","ROR","RCL","RCR","BT","BTS","BTR","BTC").contains(m))return "integer_logic_shift";
        if(Arrays.asList("ADD","ADC","SUB","SBB","INC","DEC","NEG","MUL","IMUL","DIV","IDIV","CDQ","CWD","CBW","CWDE").contains(m))return "integer_arithmetic";
        if(m.startsWith("MOV")||m.equals("LEA")||m.startsWith("PUSH")||m.startsWith("POP")||m.equals("XCHG"))return "data_movement_or_stack";
        return "other";
    }
    private Map<String,Object> functionShape(Function fn,RandomAccessFile file)throws Exception{
        AddressSetView body=fn.getBody();long bytes=body.getNumAddresses();if(bytes<1||bytes>MAX_BODY-totalBody)throw new IllegalArgumentException("Selected body cap");totalBody+=(int)bytes;
        MessageDigest memoryHash=MessageDigest.getInstance("SHA-256"),fileHash=MessageDigest.getInstance("SHA-256");
        for(AddressRange r:body.getAddressRanges()){
            if(!original(r.getMinAddress())||!original(r.getMaxAddress()))throw new IllegalArgumentException("Body outside original domain");
            Address a=r.getMinAddress();long left=r.getLength();
            while(left>0){monitor.checkCancelled();MemoryBlock block=currentProgram.getMemory().getBlock(a);MemoryBlockSourceInfo source=null;
                if(block==null||!block.isExecute())throw new IllegalArgumentException("Body not executable memory");
                for(MemoryBlockSourceInfo s:block.getSourceInfos())if(s.contains(a)&&s.getFileBytesOffset(a)>=0){source=s;break;}
                if(source==null)throw new IllegalArgumentException("Body lacks original file provenance");
                int z=(int)Math.min(4096,Math.min(left,source.getMaxAddress().subtract(a)+1));byte[] actual=new byte[z],expected=new byte[z];
                if(currentProgram.getMemory().getBytes(a,actual)!=z)throw new IllegalArgumentException("Short program memory");file.seek(source.getFileBytesOffset(a));file.readFully(expected);
                if(!Arrays.equals(actual,expected))throw new IllegalArgumentException("Selected original byte mismatch at "+a);memoryHash.update(actual);fileHash.update(expected);left-=z;if(left>0)a=a.addNoWrap(z);
            }
        }
        AddressSet covered=new AddressSet();Map<String,Integer> classes=new TreeMap<>(),mnemonics=new TreeMap<>(),pcode=new TreeMap<>(),memory=new TreeMap<>(),flow=new TreeMap<>(),memoryBasis=memoryBasis();List<Map<String,Object>> debts=new ArrayList<>();int count=0,coveredBytes=0,x87=0,compares=0,conditional=0,returns=0;
        InstructionIterator it=currentProgram.getListing().getInstructions(body,true);
        while(it.hasNext()){
            monitor.checkCancelled();Instruction i=it.next();if(count>=MAX_INSTRUCTIONS)throw new IllegalArgumentException("Instruction cap");count++;
            if(!body.contains(i.getMinAddress(),i.getMaxAddress()))throw new IllegalArgumentException("Instruction straddles analyzed range");covered.addRange(i.getMinAddress(),i.getMaxAddress());coveredBytes+=i.getLength();
            String m=i.getMnemonicString().toUpperCase(Locale.ROOT),cl=opcodeClass(i);inc(classes,cl);inc(mnemonics,m);if(cl.equals("x87"))x87++;
            if(m.equals("CMP")||m.equals("TEST")||m.startsWith("FCOM")||m.startsWith("FUCOM")||m.startsWith("COMIS")||m.startsWith("UCOMIS"))compares++;
            if(i.getFlowType().isConditional()&&i.getFlowType().isJump())conditional++;if(cl.equals("return"))returns++;
            if(i.getFlowType().isComputed()&&!cl.equals("return"))debts.add(rec("kind","computed-flow-analysis-only","site",va(i.getAddress()),"flow",i.getFlowType().toString()));
            PcodeOp[] ops=i.getPcode();if(ops.length>1024||ops.length>MAX_PCODE-totalPcode)throw new IllegalArgumentException("Pcode cap");totalPcode+=ops.length;Map<String,PcodeOp> defs=new HashMap<>();
            for(PcodeOp op:ops){inc(pcode,op.getMnemonic());if(op.getOutput()!=null)defs.put(vnKey(op.getOutput()),op);}
            accountMemory(ops,defs,memory,memoryBasis);
        }
        AddressSet gaps=new AddressSet(body);gaps.delete(covered);if(!gaps.isEmpty())debts.add(rec("kind","analyzed-body-bytes-without-instruction","bytes",gaps.getNumAddresses()));
        List<Map<String,Object>> blocks=new ArrayList<>(),edges=new ArrayList<>();BasicBlockModel model=new BasicBlockModel(currentProgram);CodeBlockIterator bi=model.getCodeBlocksContaining(body,monitor);
        while(bi.hasNext()){
            monitor.checkCancelled();if(totalBlocks>=MAX_BLOCKS)throw new IllegalArgumentException("Block cap");CodeBlock block=bi.next();totalBlocks++;boolean owned=true;for(AddressRange r:block.getAddressRanges())if(!body.contains(r.getMinAddress(),r.getMaxAddress()))owned=false;
            if(!owned){debts.add(rec("kind","block-overlaps-outside-function-body","start",original(block.getMinAddress())?va(block.getMinAddress()):block.getMinAddress().toString()));continue;}
            blocks.add(rec("start",va(block.getFirstStartAddress()),"ranges",ranges(block),"flow",block.getFlowType().toString()));CodeBlockReferenceIterator refs=block.getDestinations(monitor);
            while(refs.hasNext()){
                monitor.checkCancelled();if(totalEdges>=MAX_EDGES)throw new IllegalArgumentException("Flow-reference cap");CodeBlockReference ref=refs.next();totalEdges++;FlowType f=ref.getFlowType();Address target=ref.getReference(),site=ref.getReferent();
                if(f.isCall()){inc(flow,"call-excluded-from-intrabody-CFG");continue;}
                if(target!=null&&body.contains(target)){String k=f.isConditional()?"conditional":f.isComputed()?"computed-analyzed":"direct-or-fallthrough";inc(flow,k);edges.add(rec("from",va(block.getFirstStartAddress()),"to",va(target),"site",site!=null&&original(site)?va(site):null,"kind",k));}
                else{inc(flow,"outside-or-unresolved");debts.add(rec("kind",f.isJump()?"outside-jump-tail-analysis-only":"outside-or-unresolved-flow","site",site!=null&&original(site)?va(site):null,"flow",f.toString(),"target",target!=null&&original(target)?va(target):target==null?null:target.toString()));}
            }
        }
        Map<String,Object> result=rec("entry",va(fn.getEntryPoint()),"program_entry",fn.getEntryPoint().toString(),"ranges",ranges(body),"body_bytes",bytes,"program_body_sha256",digest(memoryHash),"original_file_body_sha256",digest(fileHash),"body_hash_scope","concatenated analyzed ranges ascending; no addresses or raw bytes","boundary_status","analyzed-body-only","instruction_count",count,"instruction_bytes",coveredBytes,"instruction_gap_ranges",ranges(gaps),"opcode_classes",classes,"mnemonic_counts",mnemonics,"pcode_classes",pcode,"memory_pcode_width_classes",memory,"memory_basis_counts",memoryBasis,"memory_accounting_schema","pcode-data-memory-accounting-v2","memory_classification","raw Pcode operation/varnode occurrences, not exact machine access counts; direct nonflow RAM inputs/outputs are absolute; per-op duplicate inputs dedup; flow operands excluded; LOAD/STORE intrinsic access counted once; embedded RAM storage operands remain excluded debt; constant/stack-only address derivation for indirect access","x87_count",x87,"compare_count",compares,"conditional_branch_count",conditional,"return_count",returns,"CFG",rec("model","BasicBlockModel","owned_blocks",blocks,"intrabody_edges",edges,"flow_counts",flow,"debts",debts,"complete_original_CFG",false),"review_flags",rec("identity",false,"ABI",false,"extent",false,"type",false),"purpose","auxiliary candidate shape; not independent support roots");
        if(json.toJson(result).getBytes(StandardCharsets.UTF_8).length>MAX_ROW)throw new IllegalArgumentException("Function row cap");return result;
    }
    public void run()throws Exception{
        String[] a=getScriptArgs();if(a.length!=4)throw new IllegalArgumentException("output.json original signed-delta selectors.json");Path output=Paths.get(a[0]).toAbsolutePath(),original=Paths.get(a[1]),config=Paths.get(a[3]);delta=Long.decode(a[2]);
        if(Files.exists(output))throw new IllegalArgumentException("Output exists");if(Files.size(config)>MAX_ROW)throw new IllegalArgumentException("Selector row cap");String cfgHash=hash(config),srcHash=hash(Paths.get(getSourceFile().getAbsolutePath()));byte[] cfgBytes;try(InputStream input=Files.newInputStream(config)){cfgBytes=input.readNBytes(MAX_ROW+1);}if(cfgBytes.length>MAX_ROW)throw new IllegalArgumentException("Selector row cap");JsonObject cfg=json.fromJson(new String(cfgBytes,StandardCharsets.UTF_8),JsonObject.class);
        if(!"selected-function-shape-selectors-v1".equals(cfg.get("schema").getAsString())||cfg.get("delta").getAsLong()!=delta)throw new IllegalArgumentException("Selector schema/delta");
        binaryHash=hash(original);if(!binaryHash.equalsIgnoreCase(cfg.get("binary_sha256").getAsString())||!binaryHash.equalsIgnoreCase(currentProgram.getExecutableSHA256()))throw new IllegalArgumentException("Original/program/config SHA mismatch");
        if(!currentProgram.getName().equals(cfg.get("program").getAsString()))throw new IllegalArgumentException("Wrong program");long base=imageBase(original);if(currentProgram.getImageBase().getOffset()+delta!=base)throw new IllegalArgumentException("Delta does not match original header base");
        if(currentProgram.getDefaultPointerSize()!=4||currentProgram.getLanguage().isBigEndian())throw new IllegalArgumentException("Need LE32 program");stackPointer=currentProgram.getCompilerSpec().getStackPointer();memoryAccountingSelfTest();JsonArray entries=cfg.getAsJsonArray("entries");if(entries.size()<1||entries.size()>MAX_FUNCTIONS)throw new IllegalArgumentException("Function selector cap");Set<Long> unique=new TreeSet<>();for(JsonElement v:entries){long value=Long.decode(v.getAsString());if(!unique.add(value))throw new IllegalArgumentException("Duplicate entry");}
        List<Map<String,Object>> functions=new ArrayList<>();try(RandomAccessFile f=new RandomAccessFile(original.toFile(),"r")){for(long value:unique){monitor.checkCancelled();Address addr=currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(value-delta);Function fn=currentProgram.getFunctionManager().getFunctionAt(addr);if(fn==null||fn.isExternal()||!original(addr))throw new IllegalArgumentException("Missing exact original function entry "+Long.toHexString(value));functions.add(functionShape(fn,f));}}
        if(!hash(original).equals(binaryHash)||!hash(config).equals(cfgHash)||!hash(Paths.get(getSourceFile().getAbsolutePath())).equals(srcHash))throw new IllegalArgumentException("Input changed");
        Map<String,Object> result=rec("schema","selected-function-shapes-v2","memory_accounting_schema","pcode-data-memory-accounting-v2","self_tests",rec("memory_accounting","PASS","fixtures",7),"status","complete-selected-candidate-only-export","binary_sha256",binaryHash,"binary_md5",currentProgram.getExecutableMD5(),"side",cfg.get("side").getAsString(),"program",currentProgram.getName(),"tool","Ghidra "+Application.getApplicationVersion(),"language",currentProgram.getLanguageID().toString(),"compiler",currentProgram.getCompilerSpec().getCompilerSpecID().toString(),"analysis","existing-program-readonly-noanalysis-required","address_transform",rec("delta",delta,"program_image_base",currentProgram.getImageBase().toString(),"original_image_base",String.format("0x%08x",base),"original_header_verified",true),"selector_sha256",cfgHash,"exporter_sha256",srcHash,"selected_entries",unique.stream().map(v->String.format("0x%08x",v)).toList(),"functions",functions,"limits",rec("functions",MAX_FUNCTIONS,"instructions_per_function",MAX_INSTRUCTIONS,"selected_body_bytes",MAX_BODY,"blocks",MAX_BLOCKS,"flow_references",MAX_EDGES,"pcode_ops",MAX_PCODE,"function_row_bytes",MAX_ROW,"output_bytes",MAX_OUTPUT),"used",rec("functions",functions.size(),"selected_body_bytes",totalBody,"blocks",totalBlocks,"flow_references",totalEdges,"pcode_ops",totalPcode),"limitations",Arrays.asList("No raw code bytes emitted","Analyzed ranges, not independently reviewed full extents","No original semantic/ABI proof","Computed flow and exterior tails remain explicit debt","Memory classes are conservative Pcode shapes, not aggregate types","No PIC adjustment or name-based feature credit","Auxiliary features never independent roots or reviewed anchors"));
        byte[] encoded=(json.toJson(result)+"\n").getBytes(StandardCharsets.UTF_8);if(encoded.length>MAX_OUTPUT)throw new IllegalArgumentException("Output cap");Path parent=output.getParent();Files.createDirectories(parent);Path temp=Files.createTempFile(parent,".shape-pending-",".json");try{Files.write(temp,encoded);try(FileChannel ch=FileChannel.open(temp,StandardOpenOption.WRITE)){ch.force(true);}Files.createLink(output,temp);}finally{Files.deleteIfExists(temp);}println("Selected shape export complete: "+output);
    }
}
