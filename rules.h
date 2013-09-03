/*			REGOLE PREDEFINITE PER AUTOMA			*/
/*	Le regole si definiscono in notazione MCell:			*/
/*	Sp/Bq	dove b e' il numero di vicini vivi necessari affinche'	*/
/*		una cella sopravviva, q e' il numero di vivi necessari	*/
/*		affinche' una cellula morta rinasca			*/



//	typedef struct {
//		short rule_s[8];	es: {2,3} per S23
//		short rule_b[8];	es: {3} per B3
//		short  size_rule_s;	es: 2 per S23
//		short  size_rule_b;	es: 1 per B3
//		short sierpinski;	1 se si tratta dell'automa 1D,
//					0 altrimenti
//	} rule;



rule game_of_life	= { {2,3},		{3},		2,	1,	0 };
rule three4_life	= { {3,4},		{3,4},		2,	2,	0 };
rule high_life		= { {3,2},		{3,6},		2,	2,	0 };
rule day_night		= { {3,4,6,7,8},	{3,6,7,8},	5,	4,	0 };
rule sierp		= { {0},		{8},		1,	1,	1 };

rule* rules = { &game_of_life,
		&three4_life,
		&high_life,
		&day_night,
		&sierp
		};