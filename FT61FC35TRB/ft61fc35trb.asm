//Deviec:FT61FC3X
//-----------------------Variable---------------------------------
//-----------------------Variable END---------------------------------

		BCR 	PCLATH,3 		//0000 	118A
		LJUMP 	0BH 			//0001 	380B
		ORG		0004H
		STR 	7EH 			//0004 	01FE
		SWAPR 	STATUS,0 		//0005 	0703
		STR 	70H 			//0006 	01F0
		LDR 	PCLATH,0 		//0007 	080A
		STR 	71H 			//0008 	01F1
		BCR 	PCLATH,3 		//0009 	118A
		LJUMP 	10H 			//000A 	3810
		BCR 	PCLATH,3 		//000B 	118A
		LJUMP 	0DH 			//000C 	380D
		CLRR 	STATUS 			//000D 	0103
		BCR 	PCLATH,3 		//000E 	118A
		LJUMP 	0BH 			//000F 	380B
		LDR 	71H,0 			//0010 	0871
		STR 	PCLATH 			//0011 	018A
		SWAPR 	70H,0 			//0012 	0770
		STR 	STATUS 			//0013 	0183
		SWAPR 	7EH,1 			//0014 	07FE
		SWAPR 	7EH,0 			//0015 	077E
		RETI		 			//0016 	0009
		BCR 	PCLATH,3 		//0017 	118A
		LJUMP 	0BH 			//0018 	380B
			END
