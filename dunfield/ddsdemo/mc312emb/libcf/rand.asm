*
* Generate a pseudo-random number: int rand(limit)
*
rand	LD	RANDSEED	Get current seed
	MUL	#13709		First calculation
	ADD	#13849		Second calculation
	ST	RANDSEED	Resave seed
	DIV	2,S		Limit with division
	ALT			Get remainder
	RET
$DD:RANDSEED 2
