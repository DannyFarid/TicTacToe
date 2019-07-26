#include "ccc_win.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

string x_moves, o_moves, computer_moves;

void setup_grid();
void reset(bool &winner, bool &x_turn);
void make_move(string player, bool &winner, bool &x_turn);
int mode_selection();
int difficulty_selection();
int draw_x(Point C);
int draw_circle(Point C);
int top_horiz(string winner);
int mid_horiz(string winner);
int bottom_horiz(string winner);
int left_vert(string winner);
int mid_vert(string winner);
int right_vert(string winner);
int left_diag(string winner);
int right_diag(string winner);
int check_winner(string moves, string player);
int AI_easy(bool &winner, bool x_turn);
int AI_medium(bool &winner, bool x_turn);
int AI_hard(bool &winner, bool x_turn);
bool can_win(string pos1, string pos2, string pos3, Point &center, Point p1, Point p2, Point p3);
bool can_block(string pos1, string pos2, string pos3, Point &center, Point p1, Point p2, Point p3);
bool can_play(string square);
bool can_go(int counter, bool winner);
string get_square(double x, double y);

int ccc_win_main() {

	srand((int)time(0)); //sets random seed for the operation of the artifical intelligence part
	bool play = true;

	while (play) {

		int counter = 0, mode, difficulty;
		bool winner = false, x_turn = true;

		reset(winner, x_turn);
		mode = mode_selection(); //mode set by the return value of mode_selection function (1 or 2)

		if (mode == 1)
			difficulty = difficulty_selection(); //difficulty set by return value of difficulty_selection function (0, 1, or 2)

		setup_grid(); //sets up empty tic-tac-toe board

		while (can_go(counter, winner)) //this will run as long as there is no winner and less than 9 turns have been made
		{
			while (x_turn && can_go(counter, winner)) //x sequence
			{
				make_move("x", winner, x_turn); //see make_move function
				counter++;
			}
			if (mode == 2 && can_go(counter, winner)) //if in 2 player mode and no winner, regular o sequence runs
			{
				while (!x_turn && can_go(counter, winner))  //o sequence
				{
					make_move("o", winner, x_turn);//see make_move function
					counter++;
				}
			}
			else if (mode == 1 && can_go(counter, winner)) //if in 1 player mode, o sequence will be skipped and ai sequence will run
			{
				while (!x_turn && can_go(counter, winner))
				{
					if (difficulty == 0) AI_easy(winner, x_turn);
					else if (difficulty == 1) AI_medium(winner, x_turn);
					else if (difficulty == 2) AI_hard(winner, x_turn);
					x_turn = true;
					counter++;
				}
			}
		}

		if (!winner) winner = check_winner(x_moves, "X");

			//if someone wins on the last turn, the two check_winner commands above will not run because
			//we will have exited the while loop already; this checks for a winner one last time
			//(if there is not already a winner)
			
		if (!winner) cwin << Message(Point(-1, 8.75), "Draw!");

			//if still no winner, say Draw		

		string play_again = cwin.get_string("Game Over! Do you want to play again? (Y/N)"); //play again sequence

		if (play_again == "Y" || play_again == "y") play = true;
		//if play stays true, then start all over again
		else play = false;
		//if play becomes false, exit the outer-most while loop

		cwin.clear();
	}

	cwin << Message(Point(-2, 0), "Thanks for playing!");
	return 0;
} 

void setup_grid() {

	/*************************************
	sets up the tic-tac-toe grid of lines
	**************************************/

	cwin.clear();
	Line x1(Point(3, -9), Point(3, 9));
	Line x2(Point(-3, -9), Point(-3, 9));
	Line y1(Point(-9, 3), Point(9, 3));
	Line y2(Point(-9, -3), Point(9, -3));
	cwin << x1 << x2 << y1 << y2;
}

int draw_x(Point C)
{

	/********************************************************
	draws the x (box letter) centered around a certain point
	*********************************************************/

	int x_coordC = C.get_x();
	int y_coordC = C.get_y();
	Line L1(Point(x_coordC, y_coordC + 0.5), Point(x_coordC - 1, y_coordC + 2));
	Line L2(Point(x_coordC, y_coordC + 0.5), Point(x_coordC + 1, y_coordC + 2));
	Line L3(Point(x_coordC + 1, y_coordC + 2), Point(x_coordC + 2, y_coordC + 2));
	Line L4(Point(x_coordC + 2, y_coordC + 2), Point(x_coordC + 0.5, y_coordC));
	Line L5(Point(x_coordC + 0.5, y_coordC), Point(x_coordC + 2, y_coordC - 2));
	Line L6(Point(x_coordC + 2, y_coordC + -2), Point(x_coordC + 1, y_coordC - 2));
	Line L7(Point(x_coordC + 1, y_coordC - 2), Point(x_coordC, y_coordC - 0.5));
	Line L8(Point(x_coordC, y_coordC - 0.5), Point(x_coordC - 1, y_coordC - 2));
	Line L9(Point(x_coordC - 1, y_coordC - 2), Point(x_coordC - 2, y_coordC - 2));
	Line L10(Point(x_coordC - 2, y_coordC - 2), Point(x_coordC - 0.5, y_coordC));
	Line L11(Point(x_coordC - 0.5, y_coordC), Point(x_coordC - 2, y_coordC + 2));
	Line L12(Point(x_coordC - 2, y_coordC + 2), Point(x_coordC - 1, y_coordC + 2));
	cwin << L1 << L2 << L3 << L4 << L5 << L6 << L7 << L8 << L9 << L10 << L11 << L12;
	return 0;
}

int draw_circle(Point C)
{
	/*************************************************
	draws donut shape centered around a certain point
	**************************************************/

	Circle C1(C, 2);
	Circle C2(C, 1);
	cwin << C1 << C2;
	return 0;
}

void make_move(string player, bool &winner, bool &x_turn) {

	/**************************************************************************************
	general turn sequence; asks user for a click (or generates random "click" when computer
	is playing); logs each player's move in their respective moves log; checks for a winner
	each time (only after 2 moves have been played, because a win isn't possible before that)
	****************************************************************************************/

	Point click, center, A(-6, 6), B(0, 6), C(6, 6), D(-6, 0), E(0, 0), F(6, 0), G(-6, -6), H(0, -6), I(6, -6);
	double x_coord, y_coord;
	string square;

	if (player != "c")
	{
		do {

			if (player == "x")
				click = cwin.get_mouse("X's turn!");
			else if (player == "o")
				click = cwin.get_mouse("O's turn!");
			x_coord = click.get_x();
			y_coord = click.get_y();

			square = get_square(x_coord, y_coord);

			if (!can_play(square))
				cwin.get_int("Someone has already gone here! Click enter to try again.");

		} while (!can_play(square));
	}

	if (player == "c")
	{
		do {

			if (player == "c") {
				x_coord = rand() % (19) - 9;
				y_coord = rand() % (19) - 9;
				while (x_coord == 3 || x_coord == -3 || x_coord == 9 || x_coord == -9 || y_coord == 3 || y_coord == -3 || y_coord == 9 || y_coord == -9)
				{
					x_coord = rand() % (19) - 9;
					y_coord = rand() % (19) - 9;
				}
			}

			square = get_square(x_coord, y_coord);

		} while (!can_play(square));
	}

	//each if statement makes sure that the chosen square is empty before proceeding
	if (square == "a"  && can_play("a")) {
		center = A;
	}
	else if (square == "b" && can_play("b")) {
		center = B;
	}
	else if (square == "c" && can_play("c")) {
		center = C;
	}
	else if (square == "d" && can_play("d")) {
		center = D;
	}
	else if (square == "e" && can_play("e")) {
		center = E;
	}
	else if (square == "f" && can_play("f")) {
		center = F;
	}
	else if (square == "g" && can_play("g")) {
		center = G;
	}
	else if (square == "h" && can_play("h")) {
		center = H;
	}
	else if (square == "i" && can_play("i")) {
		center = I;
	}

	if (player == "x") { //draw x and log x's move
		x_moves += square;
		draw_x(center);
		if (x_moves.length() >= 2) winner = check_winner(x_moves, "X");
	}
	else {
		draw_circle(center); //draw circle and log o's or computer's move
		if (player == "o") {
			o_moves += square;
			if (o_moves.length() >= 2) winner = check_winner(o_moves, "O");
		}
		else if (player == "c") {
			computer_moves += square;
			if (computer_moves.length() >= 2) winner = check_winner(computer_moves, "Computer");
		}
	}

	if (player == "x")
		x_turn = false;
	else
		x_turn = true;
}

int AI_medium(bool &winner, bool x_turn) {

/******************************************************************************
computer turn sequence; if can block, it will block; if can win, it will win;
computer prioritizes winning if both a win and block is possible; otherwise makes
random move
*******************************************************************************/
	
	bool win = false;
	Point center, A(-6, 6), B(0, 6), C(6, 6), D(-6, 0), E(0, 0), F(6, 0), G(-6, -6), H(0, -6), I(6, -6);

	if (x_moves.length() >= 2) 
	{
		if (can_win("a", "b", "c", center, A, B, C) ||
			can_win("d", "e", "f", center, D, E, F) ||
			can_win("g", "h", "i", center, G, H, I) ||
			can_win("a", "d", "g", center, A, D, G) ||
			can_win("b", "e", "h", center, B, E, H) ||
			can_win("c", "f", "i", center, C, F, I) ||
			can_win("a", "e", "i", center, A, E, I) ||
			can_win("c", "e", "g", center, C, E, G) ||
			can_block("a", "b", "c", center, A, B, C) ||
			can_block("d", "e", "f", center, D, E, F) ||
			can_block("g", "h", "i", center, G, H, I) ||
			can_block("a", "d", "g", center, A, D, G) ||
			can_block("b", "e", "h", center, B, E, H) ||
			can_block("c", "f", "i", center, C, F, I) ||
			can_block("a", "e", "i", center, A, E, I) ||
			can_block("c", "e", "g", center, C, E, G))
			win = true;
	}

	if (!win)
		make_move("c", winner, x_turn);
	else draw_circle(center);

	winner = check_winner(computer_moves, "Computer"); //checks for winner each time
	return 0;
}

int AI_easy(bool &winner, bool x_turn) {

	/*****************************************************************************
	computer turn sequence; each move is just random, hence the "easy" difficulty
	******************************************************************************/

	make_move("c", winner, x_turn);
	winner = check_winner(computer_moves, "Computer");

	return 0;
}

int AI_hard(bool &winner, bool x_turn) {

	
	
	Point center, A(-6, 6), B(0, 6), C(6, 6), D(-6, 0), E(0, 0), F(6, 0), G(-6, -6), H(0, -6), I(6, -6);
	bool went = false;

/******************************************************************************
computer turn sequence; it is almost impossible to beat the computer in this mode;
computer will usually either win or draw; even if played perfectly, still an 80%
chance that computer will win
*******************************************************************************/

	/****************************************************************************
	first 4 if statements are hard-coded moves the computer should make in response
	to certain moves made by the player
	******************************************************************************/
	
	if (x_moves.find("e") != -1 && x_moves.length() == 1) {
		center = C;
		computer_moves += "c";
		went = true;
	}
	else if (x_moves.find("e") == -1 && x_moves.length() == 1) {
		center = E;
		computer_moves += "e";
		went = true;
	}
	else if (x_moves.find("c") != -1 && x_moves.find("g") != -1 && x_moves.length() == 2) {
		center = B;
		computer_moves += "b";
		went = true;
	}
	else if (x_moves.find("g") != -1 && x_moves.length() == 2 && x_moves.find("d") == -1) {
		center = I;
		computer_moves += "i";
		went = true;
	}

	if (went) draw_circle(center); //if one of the four hard-coded moves were triggered above, make the move
	else AI_medium(winner, x_turn); //if none of the above were triggered, run regular AI algorithm in AI_medium

	return 0;
}

int check_winner(string moves, string player) {
	/*******************************************************************************************************
	checks for winner (either X, O or computer), and if there is one, returns true; otherwise returns false
	********************************************************************************************************/

	/*************************************************************************************
	this works based on the principle that if trying to .find some substring that does not
	appear in the original string, the return will be -1, and if it does appear, it will be
	something other than -1 (>= 0); each letter corresponds to a square on the board (going
	left to right, top to bottom like reading a book)
	**************************************************************************************/
	int a = moves.find("a", 0);
	int b = moves.find("b", 0);
	int c = moves.find("c", 0);
	int d = moves.find("d", 0);
	int e = moves.find("e", 0);
	int f = moves.find("f", 0);
	int g = moves.find("g", 0);
	int h = moves.find("h", 0);
	int i = moves.find("i", 0);
	int win = false;

	/*******************************************************************************************
	all if statements below check for three consecutive filled in squares; for example, the first
	if statements checks if there is an "a," "b," and a "c" in the string "moves." if there is, this
	means that there is a win on the top row; "moves" will be a log of either x"s, o"s, or the
	computer"s moves depending on the input

	if a win is found, the corresponding function to draw the winning line is called
	*********************************************************************************************/

	if (a >= 0 && b >= 0 && c >= 0) {
		top_horiz(player);
		win = true;
	}
	if (d >= 0 && e >= 0 && f >= 0) {
		mid_horiz(player);
		win = true;
	}
	if (g >= 0 && h >= 0 && i >= 0) {
		bottom_horiz(player);
		win = true;
	}
	if (a >= 0 && d >= 0 && g >= 0) {
		left_vert(player);
		win = true;
	}
	if (b >= 0 && e >= 0 && h >= 0) {
		mid_vert(player);
		win = true;
	}
	if (c >= 0 && f >= 0 && i >= 0) {
		right_vert(player);
		win = true;
	}
	if (a >= 0 && e >= 0 && i >= 0) {
		left_diag(player);
		win = true;
	}
	if (c >= 0 && e >= 0 && g >= 0) {
		right_diag(player);
		win = true;
	}
	return win;
}

bool can_win(string pos1, string pos2, string pos3, Point &center, Point p1, Point p2, Point p3) {

	/*************************************************************
	checks if the computer can win with any move; if it can, it will
	make that move and return true; if it can not, will return false
	***************************************************************/

	
	if (computer_moves.find(pos1) != -1 && computer_moves.find(pos2) != -1 && can_play(pos3)) {
		center = p3;
		computer_moves += pos3;
		return true;
	}
	else if (computer_moves.find(pos2) != -1 && computer_moves.find(pos3) != -1 && can_play(pos1)) {
		center = p1;
		computer_moves += pos1;
		return true;
	}
	else if (computer_moves.find(pos1) != -1 && computer_moves.find(pos3) != -1 && can_play(pos2)) {
		center = p2;
		computer_moves += pos2;
		return true;
	}
	else return false;
}

bool can_block(string pos1, string pos2, string pos3, Point &center, Point p1, Point p2, Point p3) {

	/******************************************************************
	checks if the computer can block a win with any move; if it can, it 
	will make that move and return true; if it can not, will return false
	********************************************************************/

	if (x_moves.find(pos1) != -1 && x_moves.find(pos2) != -1 && can_play(pos3)) {
		center = p3;
		computer_moves += pos3;
		return true;
	}
	else if (x_moves.find(pos2) != -1 && x_moves.find(pos3) != -1 && can_play(pos1)) {
		center = p1;
		computer_moves += pos1;
		return true;
	}
	else if (x_moves.find(pos1) != -1 && x_moves.find(pos3) != -1 && can_play(pos2)) {
		center = p2;
		computer_moves += pos2;
		return true;
	}
	else return false;
}

int mode_selection() {

/***********************************************
mode selection interface (1 player or 2 players)
************************************************/

	cwin << Message(Point(-4.5, 3), "Please select 1 Player or 2 Players") << Message(Point(-3, 0), "1 Player") << Message(Point(0.7, 0), "2 Players");
	Point selection = cwin.get_mouse("");
	if (selection.get_x() < 0)
		return 1;
	else
		return 2;
}

int difficulty_selection() {
	
/********************************************************
difficulty selection interface (easy, medium, or hard)
*********************************************************/

	cwin.clear();
	cwin << Message(Point(-2.5, 3), "Please select difficulty") << Message(Point(-3, 0), "Easy") << Message(Point(-0.5, 0), "Medium") << Message(Point(2.5, 0), "Hard");
	Point selection = cwin.get_mouse("");

	if (selection.get_x() <= -1.5)
		return 0;
	else if (selection.get_x() > -1.5 && selection.get_x() <= 1.75)
		return 1;
	else if (selection.get_x() > 1.75)
		return 2;
	else cwin.get_int("You clicked out of bounds :(");
}

bool can_play(string square) {
	
	/************************************************
	checks if a move can be made in a certain square
	(i.e. it's not occupied by anything)
	*************************************************/
	
	if (x_moves.find(square,0) <= 10 || o_moves.find(square,0) <= 10 || computer_moves.find(square,0) <= 10)
		return false;
	else return true;
}

bool can_go(int counter, bool winner) {

	/*****************************************
	checks if game can continue being played
	(i.e. less than 9 moves have been made and
	there is not a winner yet
	******************************************/

	if (!winner && counter < 9)
		return true;
	else return false;
}

void reset(bool &winner, bool &x_turn) {

	/*******************************************************************************************
	resets everything that needs to be reset for game to start over (global variables included)
	********************************************************************************************/

	x_moves = "";
	o_moves = "";
	computer_moves = "";
	winner = false;
	x_turn = true;
}

string get_square(double x, double y) {
	
	/************************************************************
	given an x_coord and a y_coord, returns which square we are in
	*************************************************************/
	
	if (x < -3 && y > 3) {
		return "a";
	}
	else if (x > -3 && x < 3 && y > 3) {
		return "b";
	}
	else if (x > 3 && y > 3) {
		return "c";
	}
	else if (x < -3 && y < 3 && y > -3) {
		return "d";
	}
	else if (x > -3 && x < 3 && y < 3 && y > -3) {
		return "e";
	}
	else if (x > 3 && y < 3 && y > -3) {
		return "f";
	}
	else if (x < -3 && y < -3) {
		return "g";
	}
	else if (x > -3 && x < 3 && y < -3) {
		return "h";
	}
	else if (x > 3 && y < -3) {
		return "i";
	}
	else cwin.get_int("You clicked out of bounds :(");
}

/***********************************************************
the last 8 functions draw lines for different ways to win
************************************************************/

int top_horiz(string winner)
{
	/*******************************************
	draws line indicating a win on the top row
	********************************************/

	Line L1(Point(-8, 5.5), Point(-8, 6.5));
	Line L2(Point(8, 5.5), Point(8, 6.5));
	Line L3(Point(-8, 5.5), Point(8, 5.5));
	Line L4(Point(-8, 6.5), Point(8, 6.5));
	string win = winner + " wins!";
	cwin << Message(Point(-1, 8.75), win) << L1 << L2 << L3 << L4;
	return 0;
}
int mid_horiz(string winner)
{

	/********************************************
	draws line indicating a win on the middle row
	*********************************************/
	Line L1(Point(-8, -0.5), Point(-8, 0.5));
	Line L2(Point(8, -0.5), Point(8, 0.5));
	Line L3(Point(-8, -0.5), Point(8, -0.5));
	Line L4(Point(-8, 0.5), Point(8, 0.5));
	string win = winner + " wins!";
	cwin << Message(Point(-1, 8.75), win) << L1 << L2 << L3 << L4;
	return 0;
}
int bottom_horiz(string winner)
{
	Line L1(Point(-8, -6.5), Point(-8, -5.5));
	Line L2(Point(8, -6.5), Point(8, -5.5));
	Line L3(Point(-8, -6.5), Point(8, -6.5));
	Line L4(Point(-8, -5.5), Point(8, -5.5));
	string win = winner + " wins!";
	cwin << Message(Point(-1, 8.75), win) << L1 << L2 << L3 << L4;
	return 0;
}
int left_diag(string winner)
{
	Line L1(Point(-8, 7), Point(-7, 8));
	Line L2(Point(7, -8), Point(8, -7));
	Line L3(Point(7, -8), Point(-8, 7));
	Line L4(Point(8, -7), Point(-7, 8));
	string win = winner + " wins!";
	cwin << Message(Point(-1, 8.75), win) << L1 << L2 << L3 << L4;
	return 0;
}
int right_diag(string winner)
{
	Line L1(Point(-7, -8), Point(-8, -7));
	Line L2(Point(7, 8), Point(8, 7));
	Line L3(Point(-8, -7), Point(7, 8));
	Line L4(Point(-7, -8), Point(8, 7));
	string win = winner + " wins!";
	cwin << Message(Point(-1, 8.75), win) << L1 << L2 << L3 << L4;
	return 0;
}
int left_vert(string winner)
{
	Line L1(Point(-6.5, 8), Point(-5.5, 8));
	Line L2(Point(-6.5, -8), Point(-5.5, -8));
	Line L3(Point(-6.5, -8), Point(-6.5, 8));
	Line L4(Point(-5.5, -8), Point(-5.5, 8));
	string win = winner + " wins!";
	cwin << Message(Point(-1, 8.75), win) << L1 << L2 << L3 << L4;
	return 0;
}
int mid_vert(string winner)
{
	Line L1(Point(-0.5, -8), Point(0.5, -8));
	Line L2(Point(-0.5, 8), Point(0.5, 8));
	Line L3(Point(-0.5, -8), Point(-0.5, 8));
	Line L4(Point(0.5, -8), Point(0.5, 8));
	string win = winner + " wins!";
	cwin << Message(Point(-1, 8.75), win) << L1 << L2 << L3 << L4;
	return 0;
}
int right_vert(string winner)
{
	Line L1(Point(5.5, -8), Point(6.5, -8));
	Line L2(Point(5.5, 8), Point(6.5, 8));
	Line L3(Point(5.5, 8), Point(5.5, -8));
	Line L4(Point(6.5, -8), Point(6.5, 8));
	string win = winner + " wins!";
	cwin << Message(Point(-1, 8.75), win) << L1 << L2 << L3 << L4;
	return 0;
}

