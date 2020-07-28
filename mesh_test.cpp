#include <stdio.h>
#include <string.h>
#include <thread>
#include <mutex>
#include "serial_port.h"
#include "mesh.h"

SerialPort serial1("/dev/cu.usbmodem206C34864D4B1", 115200);
SerialPort serial2("/dev/cu.usbmodem206C34904D4B1",115200);

void
signal_callback_handler(int signum)
{
    serial1.disconnect();
    serial2.disconnect();
    exit(signum);
}

std::mutex  mtx;
int main() {


    signal(SIGINT, signal_callback_handler);


    serial1.connect();
    serial2.connect();

    std::thread([]{
        int destinationId = 35;
        MeshParser meshParser;
        meshParser.isMesh4Bytes = false; // 2bytes for the id
        meshParser.isEmitMeshSourceId = true; // received data is structured mesh data

        uint8_t datar[1024] ;
        int k = 0;
        char buf[1024] = {0};
        char *data = "1234567890abcdefghijklmnopqrstuvwxyz";
        while(true) {
            int l = serial1.receive((uint8_t *) buf, 1024);
            if(l>0) {
                meshParser.addToBuffer(datar, l);
                unsigned int sourceId = 0;
                l = meshParser.receivedMessage(datar, 1024, sourceId);

                if (l > 0) {
                    std::lock_guard<std::mutex> lock(mtx);
                    printf("%d (%dbytes): ", k, l);
                    printf("received at serial1 from source : %d \n", sourceId);
                    for (int i = 0; i < l; ++i) {
                        printf("%c", buf[i] == 0 ? ' ' : buf[i]);
                    }
                    printf("\n");
                    fflush(stdout);
                }
            }
            sprintf(buf,"-%5d %s",k++,data);

            int meshLength = meshParser.toMeshFormat((uint8_t*)buf,strlen(buf),destinationId,datar);
            serial1.send(datar, meshLength);
#define HZ_2 500000
            usleep(HZ_2);
        }

    }).detach();




    int destinationId = 25;
    MeshParser meshParser;
    meshParser.isMesh4Bytes = false; // 2bytes for the id
    meshParser.isEmitMeshSourceId = true; // received data is structured mesh data
    uint8_t datar[1024] ;
    char buf[1024] = {0};
    char *data = "1234567890abcdefghijklmnopqrstuvwxyz";
    int k = 0;
    while(true) {
        int l = serial2.receive( datar, 1024);
        if(l>0){
            meshParser.addToBuffer(datar,l);
            unsigned int sourceId = 0;
            l = meshParser.receivedMessage(datar,1024,sourceId);
            if (l > 0) {

                std::lock_guard<std::mutex> lock(mtx);
                printf("%d (%dbytes): ",k,l);

                printf("received at serial2 from source : %d \n", sourceId);
                for (int i = 0; i < l; ++i) {
                    printf("%c", (char)datar[i]);
                }
                printf("\n");
                fflush(stdout);
                memset(buf,0,sizeof(buf));
                sprintf(buf,"+%5d %s",k++,data);
                //usleep(1200);
                int meshLength = meshParser.toMeshFormat((uint8_t*)buf,strlen(buf),destinationId,datar);
                serial2.send(datar, meshLength);
            }
        }
    }


    return 0;
}