assembly = 
   {
   -- the project name
   name = "lemon-io";
   -- the project guid
   guid = "{ 0x612c9e2e, 0x2341, 0x4a7d, { 0x8a, 0x12, 0x3b, 0x2e, 0x8e, 0x58, 0xc6, 0x53 } }";
   -- the project errorcode define
   errorcode=
      {
		{ 
			name = "NOT_SUPPORT_ASYNC";
			description = "current io object not support async io";
		};
		
	    {
			name = "NOT_SUPPORT_ADDRESS_FAMILY";
		};
		
		{
			name = "INVALID_ADDRESS_STRING";
		};
		
		{
			name = "FILE_NOT_FOUND";
		};
		
		{
			name = "NOT_SUPPORT_IO_DEVICE_BOUND_FILE";
		};
		
		{
			name = "OPTION_TIMEOUT";
		};
      };
   }