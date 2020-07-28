# TestZoon
Zoon USB test codes for sending and receiving data and also parsing the mesh format


```c++
  meshParser.isMesh4Bytes = false; // 2bytes for the id
    meshParser.isEmitMeshSourceId = true; // received data is structured mesh data

    char *data = "1234567890abcdefghijklmnopqrstuvwxyz";
    char mesh_buffer[10*1024];
    unsigned int sourceId = 0;

    // adding data for destinationId: 5
    int ml = meshParser.addMesh((uint8_t *)data,strlen(data),35005,(uint8_t *)mesh_buffer);
    meshParser.addToBuffer((uint8_t *)mesh_buffer,ml);

    // adding data for destinationId: 7
    ml = meshParser.addMesh((uint8_t *)data,strlen(data),7,(uint8_t *)mesh_buffer);
    meshParser.addToBuffer((uint8_t *)mesh_buffer,ml);

    // Receiving and parsing
    while(ml = meshParser.receivedMessage((uint8_t *)mesh_buffer,1024,sourceId)){
        printf("source %d : ",sourceId);
        for(int i=0;i<ml;i++){
            printf("%c",mesh_buffer[i]);
        }
        printf("\n");
    }
```

will print 
```
source 35005 : 1234567890abcdefghijklmnopqrstuvwxyz
source 7 : 1234567890abcdefghijklmnopqrstuvwxyz
```

mesh_test.cpp is the file sends and receives with the mesh parser
here are the settings parameter  for pairs
```
{
"isMesh":true,
"meshId":35,
"isMesh4Bytes":false,
"isMeshSourceEmit":true
}
```

and

```
{
"isMesh":true,
"meshId":25,
"isMesh4Bytes":false,
"isMeshSourceEmit":true
}
```
