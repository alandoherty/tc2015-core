#ifndef SMNetwork_H
#define SMNetwork_H

typedef struct {
    char magic[4];
    int8_t opcode;
    int32_t length;
} SMPacketHeader;

typedef struct {
    int8_t opcode;
    void* data;
    int32_t size;
} SMPacket;


typedef struct {
    char inputFormat[32];
    char outputFormat[32];
    int32_t numFrames;
} SMConfig;


void* SMMakeMeAPacket(int8_t opcode, void* buffer, int32_t size) {
    // create buffer
    void* buff = malloc(size + sizeof(SMPacketHeader));
    memset(buff, 0, size + sizeof(SMPacketHeader));
    
    // header
    SMPacketHeader header;
    header.magic[0] = 'S';
    header.magic[1] = 'U';
    header.magic[2] = 'P';
    header.magic[3] = 'R';
    header.length = size;
    header.opcode = opcode;
    
    // copy header to buffer
    memcpy(buff, (void*)&header, sizeof(SMPacketHeader));
    
    // copy data
    memcpy(&buff[sizeof(SMPacketHeader)], buffer, size);
    
    return buff;
}

SMPacket* SMGiveMeAPacket(const void* buffer, int32_t size) {
    // check for idiots
    if (size < sizeof(SMPacketHeader))
        return NULL;
    
    // header
    SMPacketHeader* header = (SMPacketHeader*)malloc(sizeof(SMPacketHeader));
    memcpy(header, buffer, sizeof(SMPacketHeader));
    
    if (header->magic[0] != 'S' || header->magic[1] != 'U' || header->magic[2] != 'P' || header->magic[3] != 'R') {
        NSLog(@"[supermesh] bad packet, probably not us so...");
        free(header);
        return NULL;
    }
    
    /*printf("%x %x %x %x %x %x %x %x %x %x %x %x %x %x", ((char*)buffer)[0], ((char*)buffer)[1], ((char*)buffer)[2],
     ((char*)buffer)[3], ((char*)buffer)[4], ((char*)buffer)[5], ((char*)buffer)[6], ((char*)buffer)[7], ((char*)buffer)[8],
     ((char*)buffer)[9], ((char*)buffer)[10], ((char*)buffer)[11], ((char*)buffer)[12], ((char*)buffer)[13]);*/
    
    // create packet
    SMPacket* packet = (SMPacket*)malloc(sizeof(SMPacket));
    
    if (packet == NULL) {
        NSLog(@"[supermesh] failed to allocate packet");
        return NULL;
    }
    
    packet->opcode = header->opcode;
    packet->size = header->length;
    
    // check if length
    if (header->length > 10000000) {
        free(header);
        printf("[supermesh] packet too big: %i", header->length);
        return NULL;
    }
    
    // copy buffer to packet
    void* packetBuff = malloc(header->length);
    
    // check if successful
    if (packetBuff == NULL) {
        NSLog(@"[supermesh] failed to allocate packet data");
        free(packet);
        free(header);
        return NULL;
    }
    
    memcpy(packetBuff, &buffer[sizeof(SMPacketHeader)], header->length);
    packet->data = packetBuff;
    free(header);
    
    return packet;
}

void SMDestroyPacket(SMPacket* packet) {
    // check for idiots
    if (packet == NULL)
        return;
    
    // free
    free(packet->data);
    free(packet);
}

#endif