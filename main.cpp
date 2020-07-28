#include <stdio.h>
#include <string.h>
#include <thread>
#include <mutex>
#include "serial_port.h"
#include "mesh.h"
//#include <curses.h>

SerialPort serial1("/dev/cu.usbmodem2061348A4B4B1", 115200);
SerialPort serial2("/dev/cu.usbmodem2062349C4B4B1",115200);
MeshParser meshParser;
void
signal_callback_handler(int signum)
{
    serial1.disconnect();
    serial2.disconnect();
    exit(signum);
}


void testMesh(){
    meshParser.isMesh4Bytes = false; // 2bytes for the id
    meshParser.isEmitMeshSourceId = true; // received data is structured mesh data

    char *data = "1234567890abcdefghijklmnopqrstuvwxyz";
    char mesh_buffer[10*1024];
    unsigned int sourceId = 0;

    // adding data for destinationId: 5
    int destinationId = 55;
    int ml = meshParser.toMeshFormat((uint8_t *)data,strlen(data),destinationId,(uint8_t *)mesh_buffer);
    meshParser.addToBuffer((uint8_t *)mesh_buffer,ml);

    // adding data for destinationId: 7
    destinationId = 7;
    ml = meshParser.toMeshFormat((uint8_t *)data,strlen(data),destinationId,(uint8_t *)mesh_buffer);
    meshParser.addToBuffer((uint8_t *)mesh_buffer,ml);

    // Receiving and parsing
    while(ml = meshParser.receivedMessage((uint8_t *)mesh_buffer,1024,sourceId)){
        printf("source %d : ",sourceId);
        for(int i=0;i<ml;i++){
            printf("%c",mesh_buffer[i]);
        }
        printf("\n");
    }
}
std::mutex  mtx;
int main() {

//    testMesh();
//    return 0;
    signal(SIGINT, signal_callback_handler);
  //  SerialPort serial1("/dev/ttyACM0", 115200);

    serial1.connect();
    serial2.connect();

    std::thread([]{
        int k = 0;
        char buf[1024] = {0};
        char *data = "1234567890abcdefghijklmnopqrstuvwxyz";
        while(true) {
            int l = serial1.receive((uint8_t *) buf, 1024);
            if (l > 0) {
                std::lock_guard<std::mutex> lock(mtx);
                printf("%d (%dbytes): ",k,l);
                printf("received at serial1\n");
                for (int i = 0; i < l; ++i) {
                    printf("%c", buf[i] == 0 ? ' ' : buf[i]);
                }
                printf("\n");
                fflush(stdout);
            }
            sprintf(buf,"-%5d %s",k++,data);
           // printf("wrote: %5d \n",k);
            serial1.send((uint8_t *) buf, strlen(buf));
#define HZ_2 500000
            usleep(HZ_2);
        }

    }).detach();


//    while(true){
//        sleep(1);
//    }
    char datar[1024] ;
    char buf[1024] = {0};
    char *data = "1234567890abcdefghijklmnopqrstuvwxyz";
    int k = 0;
    while(true) {
        int l = serial2.receive((uint8_t *) datar, 1024);
        if (l > 0) {
            std::lock_guard<std::mutex> lock(mtx);
            printf("%d (%dbytes): ",k,l);

            printf("received at serial2\n");
            for (int i = 0; i < l; ++i) {
                printf("%c", datar[i]);
            }
            printf("\n");
            fflush(stdout);
            memset(buf,0,sizeof(buf));
            sprintf(buf,"+%5d %s",k++,data);
            //usleep(1200);
            serial2.send((uint8_t *) buf, strlen(buf));
        }
    }


    return 0;
}