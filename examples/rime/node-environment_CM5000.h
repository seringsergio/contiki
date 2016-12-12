

struct environment{

   int padding3[4];
   int padding2[4];
   int padding[4];
   int sequence;// To save the sequence number //tiene 2 bytes
   int temp; // To save the temperature value //tiene 2 bytes
   int light; //To save the light value //tiene 2 bytes
   rtimer_clock_t timestamp; // To save the 16-bit timestamp //tiene 2 bytes

};


