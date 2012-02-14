assembly = 
   {
   -- the project name
   name = "lemon-sys";
   -- the project guid
   guid = "{ 0x612c9e2e, 0x2341, 0x4a7d, { 0x8a, 0x11, 0x3c, 0x2e, 0x8e, 0x58, 0xc8, 0x53 } }";
   -- the project errorcode define
   errorcode=
      {
      NOT_IMPLEMENT="current code not implement";
      UUID_CREATE_ERROR = "can't create uuid";
      BUFFER_TOO_SMALL = "the buffer is too small to copy data";
      UNITTEST_FAILED = "the unittest condition check error";
      RESOURCE_ERROR = "the system resource invalid";
      INVALID_UUID_STRING = "invalid uuid string";
      LD_ERROR = "can't load share library";
      LD_GET_METHOD_ERROR = "can't get the library export function";
      COROUTINE_EXIT = "can't resume a coroutine which had exit";
      };
   }