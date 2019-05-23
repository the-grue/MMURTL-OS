/*
 * Very simple BLACKJACK (21) game
 */

#include cflea.h

#define	DECK	52
#define	SUITES	4

/*
 * Keep track of money and bet, play game & adjust total
 */
main()
{
	unsigned money, bet;
	char inbuf[11];

	money = 1000;
	for(;;) {
		printf("\nYou have %u dollars.\nHow much would you like to bet?", money);
		getstr(inbuf, 10);
		if(!(bet = atoi(inbuf))) {
			putstr("No bet - No game!\n");
			return; }
		if(bet > money) {
			putstr("You don't have that much money!");
			continue; }
		if(!(money += blackjack() ? bet : -bet)) {
			putstr("You went BUST!\n");
			return; } }
}

/*
 * Play a single 21 game
 */	
blackjack()
{
	int deck[DECK], i, j, t, player_total, dealer_total, next_card;

	player_total = dealer_total = next_card = 0;

	/* Deal a deck of cards */
	for(i=0; i < DECK; ++i)
		deck[i] = i;
	for(i=0; i < DECK; ++i) {
		t = deck[i];
		deck[i] = deck[j = rand(DECK)];
		deck[j] = t; }

	/* Accept player cards */
	for(;;) {
		printf("Player: %-4u", player_total);
		if(player_total > 21) {
			putstr("You LOSE!\n");
			return 0; }
		if(get_input("Another card (Y/N)", "YN") == 'N')
			break;
		show_card(i = deck[next_card++]);
		if(!(i %= (DECK/SUITES))) {
			if(get_input("1 or Ten?", "1T") == 'T')
				i = 9; }
		player_total += (i > 9) ? 10 : i+1; }

	/* Play dealer */
	for(;;) {
		printf("Dealer: %-4u", dealer_total);
		if(dealer_total > 21) {
			putstr("You WIN!\n");
			return 1; }
		if(dealer_total >= player_total) {
			putstr("Dealer wins!\n");
			return 0; }
		show_card(i = deck[next_card++]);
		if(!(i %= (DECK/SUITES))) {
			t = dealer_total + 10;
			if((t < 22) && ((t >= player_total) || (t < 10)))
				i = 9;
			printf("Dealer choses: %u\n", i+1); }
		dealer_total += (i > 9) ? 10 : i+1; }
}

/*
 * Get input character with prompt, and validate
 */
get_input(prompt, allowed)
	char prompt[], allowed[];
{
	int i;
	char buffer[50], *ptr, c;

	for(;;) {
		printf("%s?", prompt);
		getstr(ptr = buffer, sizeof(buffer)-1);
		while(isspace(c = toupper(*ptr)))
			++ptr;
		for(i = 0; allowed[i]; ++i)
			if(c == allowed[i])
				return c;
		printf("Huh?\n"); }
}

/*
 * Display a card
 */
show_card(card)
	int card;
{
	static char *suites[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
	static char *cards[] = { "Ace", "Two", "Three", "Four", "Five", "Six",
		"Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King" };

	printf("%s of %s\n", cards[card % (DECK/SUITES)], suites[card / (DECK/SUITES)]);
}
