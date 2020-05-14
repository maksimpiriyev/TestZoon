//
// Created by Maksim Piriyev on 5/15/20.
//

#pragma once
#include "circular_buffer.h"

class MeshParser {
public:

    circular_buffer buffer;
    bool isMesh4Bytes = false;
    bool isEmitMeshSourceId = true;

    // To hold the state of fragmented buffer data when isEmitMeshSourceId is false
    struct mesh_state {
        int state = 0;
        int length = 0;
        int remaining = 0;
        unsigned int sourceId = 0;
    } state;



    void addToBuffer(uint8_t *data,unsigned int length){
        for (int i=0; i < length; i++) {
            buffer += data[i];
        }
    }

    // Returns processed amount of buffer data
    int receivedMessage(uint8_t *parsed_data,unsigned int length, unsigned int &sourceId) {

        if (!isEmitMeshSourceId) {
            int i=0;
            for(;i<buffer.length;i++){
                parsed_data[i] = buffer[i];
            }
            buffer.skip(i);
            return i;
        }
        int j = 0;
        int k = 0;
        const int ID_LENGTH = isMesh4Bytes ? 4 : 2;
        for (int i=0; i < buffer.length; i++) {
            auto b = buffer[i];
            switch (state.state) {
                case 0:
                    if (b == 0x7E)
                        state.state++;
                    break;
                case 1:
                    if(k == 0) state.sourceId = 0;
                    state.sourceId += b << (ID_LENGTH - 1 - k) * 8;
                    k++;
                    if (k == (isMesh4Bytes ? 4 : 2)) {
                        state.state++;
                        k = 0;
                    }
                    break;
                case 2:
                    state.length = b << 8;
                    state.state++;
                    break;
                case 3:
                    state.length += b;
                    state.remaining = state.length;
                    state.state++;
                    if (state.length == 0) {
                        state.state = 0;
                    }
                    break;
                case 4:
                    parsed_data[j++] = b;
                    state.remaining--;
                    if (state.remaining <= 0) {
                        state.state = 0;
                    }
                    break;
                default:
                    break;
            }
            if (state.remaining == 0 && state.length > 0) {
                auto l = state.length;
                state.length = 0;
                sourceId = state.sourceId;
                buffer.skip(i);
                return l;
            }
        }
        if (state.remaining >= 0 && state.length > 0) {
            auto l = state.length - state.remaining;
            state.length -= state.remaining;
            buffer.skip(l);
            sourceId = state.sourceId;
            return l;
        }

        return 0;
    }


    int addMesh(uint8_t *data, int length, unsigned int destinationId, uint8_t *mesh_data) {
        int i = 0;
        mesh_data[i++] = 0x7E; // MESH MARK
        if (isMesh4Bytes) {
            mesh_data[i++] = destinationId >> 24 & 0xFF; // DESTINATION ID
            mesh_data[i++] = destinationId >> 16 & 0xFF;
            mesh_data[i++] = destinationId >> 8 & 0xFF;
            mesh_data[i++] = destinationId & 0xFF;
        } else {
            mesh_data[i++] = destinationId >> 8 & 0xFF; // DESTINATION ID
            mesh_data[i++] = destinationId & 0xFF;
        }
        mesh_data[i++] = length >> 8 & 0xFF; // LENGTH
        mesh_data[i++] = length & 0xFF;
        memcpy( mesh_data + i, data, length);
        return length + i;
    }
};


