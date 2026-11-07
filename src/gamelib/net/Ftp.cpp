#include "saga.h"

/* Network file transfer manager (retail Ftp.cpp, 0x617180-0x6179a0). Only
 * the bookkeeping survives on PC; the transfer bodies are empty. */

STATIC_ASSERT(sizeof(FtpFile) == 0xb0);
STATIC_ASSERT(offsetof(FtpFile, name) == 0xc);
STATIC_ASSERT(offsetof(FtpFile, size) == 0x8c);
STATIC_ASSERT(offsetof(FtpFile, transferred) == 0x98);
STATIC_ASSERT(offsetof(FtpFile, message_swap_endianness) == 0x9c);
STATIC_ASSERT(offsetof(FtpFile, message_data) == 0xa0);
STATIC_ASSERT(offsetof(FtpFile, transfer) == 0xac);
STATIC_ASSERT(offsetof(NetFtpManager, files) == 0x4);
STATIC_ASSERT(offsetof(NetFtpManager, field_1604) == 0x1604);
STATIC_ASSERT(sizeof(NetFtpManager) == 0x1608);
STATIC_ASSERT(offsetof(TTNetwork, ftp) == 0x2178);

i32 FtpFile::CompareAddress(const NetAddress &other) const {
    return memcmp(&other, &address, sizeof(address));
}

void NetFtpManager::Init() {
    theTTNetwork.AddListener(this, 2, "FTP");
}

const FtpFile *NetFtpManager::GetTransfers() const {
    return files;
}

void NetFtpManager::Update() {
}

void FtpFile::Init(i32 new_direction, const char *new_name, i32 new_size, const NetAddress &new_address,
                   void *new_buffer, i32 new_capacity) {
    address = new_address.address;
    direction = new_direction;
    accepted = 0;
    NuStrCpy(name, new_name);
    size = new_size;
    buffer = new_buffer;
    capacity = new_capacity;
    transferred = 0;
}

void FtpFile::Term() {
}

i32 FtpFile::Accept(i32 new_capacity, void *new_buffer) {
    switch (direction) {
        case 1:
            buffer = new_buffer;
            size = new_capacity;
            capacity = new_capacity;
            accepted = 1;
            return 0;
        case 2:
            if (size > capacity)
                return (i32)0xa0210001;
            buffer = new_buffer;
            capacity = new_capacity;
            accepted = 1;
            return 0;
        default:
            return (i32)0xa0210000;
    }
}

i32 FtpFile::Accept(i32 new_size) {
    if (new_size > capacity)
        return (i32)0xa0210001;
    size = new_size;
    accepted = 1;
    return 0;
}

i32 FtpFile::Accept() {
    accepted = 1;
    return 0;
}

FtpFile *NetFtpManager::FindTransfer(const char *name, const NetAddress &address, i32 direction) {
    for (FtpFile *file = files; file != files + 32; ++file) {
        if (file->InUse() && file->CompareAddress(address) == 0 && NuStrICmp(file->name, name) == 0 &&
            (direction == 0 || file->direction == direction))
            return file;
    }
    return NULL;
}

const FtpFile *NetFtpManager::FindTransfer(const char *name, const NetAddress &address, i32 direction) const {
    for (const FtpFile *file = files; file != files + 32; ++file) {
        if (file->InUse() && file->CompareAddress(address) == 0 && NuStrICmp(file->name, name) == 0 &&
            (direction == 0 || file->direction == direction))
            return file;
    }
    return NULL;
}

void NetFtpManager::Receive(NETMESSAGE_PC message, u8 channel, const NetAddress &address) {
    message.Release();
}

NetFtpManager::~NetFtpManager() {
    for (FtpFile *file = files; file != files + 32; ++file) {
        if (file->InUse())
            file->Close();
    }
}

void NetFtpManager::Reset() {
    for (FtpFile *file = files; file != files + 32; ++file) {
        if (file->InUse()) {
            theTTNetwork.FtpComplete(file, (i32)0xa0200001);
            file->Term();
            file->Close();
        }
    }
}

void NetFtpManager::PeerLeft(const NetAddress &address, i32 reason) {
    for (FtpFile *file = files; file != files + 32; ++file) {
        if (file->InUse() && file->CompareAddress(address) == 0) {
            theTTNetwork.FtpComplete(file, (i32)0xa0200001);
            file->Term();
            file->Close();
        }
    }
}

NetFtpManager::NetFtpManager() {
    field_1604 = 0;
}

void NetFtpManager::Term() {
    Reset();
    theTTNetwork.RemoveListener(this, 2);
}
