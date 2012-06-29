assembly = 
   {
   -- the project name
   name = "lemon-memory";
   -- the project guid
   guid = "{ 0x612c9e2e, 0x2341, 0x4a7d, { 0x8a, 0x11, 0x3b, 0x2e, 0x8e, 0x58, 0xc8, 0x53 } }";
   -- the project errorcode define
   errorcode=
      {
		{
			name = "SMALLOBJ_ALLOC_TOO_LARGE";
			description = "the required block size if out of the SmallObjAllocator max block size";
		};
      };
   }