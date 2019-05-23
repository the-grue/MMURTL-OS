/*
 * MICROCAD: One of the worlds smallest drawing programs.
 *
 * Copyright 1992-1993 Dave Dunfield
 * All rights reserved.
 *
 * Permission granted for personal (non-commercial) use only.
 *
 * Compile command: cc microcad -fop
 */
#include <stdio.h>

/* Screen dimensions */
#define	VERTICAL		480		/* Maximum screen height */
#define	HORIZONTAL		640		/* Maximum screen width */

#define	ARC_RES			64		/* Resolution of each ARC quadrant */

/* Pixel colors */
#define	SET_PIXEL		0x0F	/* Pixel will be set ON */
#define	CLEAR_PIXEL		0x00	/* Pixel will be set OFF */
#define	FLIP_PIXEL		0x8F	/* Pixel will be toggled ON/OFF */

/* Mouse buttons */
#define	MOUSE_LEFT		0x01	/* Select button */
#define	MOUSE_RIGHT		0x02	/* Cancel button */
#define	MOUSE_CENTER	0x04	/* Not used in MICROCAD */

/* Drawing buffer codes */
#define	LINE			0x01	/* Object is a LINE */
#define	BOX				0x02	/* Object is a BOX */
#define	CIRCLE			0x03	/* Object is a CIRCLE */
#define	TEXT			0x04	/* Object is a TEXT string */
#define	ARC				0x05	/* Object is an ARC */
#define	GROUP			0x06	/* Object is a GROUP of objects */
#define	ACOPY			0x07	/* Copy an object (absolute) */
#define	RCOPY			0x08	/* Copy an object (relative) */

/* Drawing storage variables */
unsigned char drawing[32000];	/* Drawing buffer */
unsigned dtop = 0, dpos;		/* Extent and position */

/*
 * Drawing parameters (saved with drawing) MUST be initialized!
 *
 * Note: In MICRO-C, we are guaranteed that initialized variables which
 * are declared together will be grouped together in storage. For other
 * compilers, you should read/write these separately to the file.
 */
#define	PBYTES	11				/* Size of parameter storage */
int grid = 0,					/* Grid spacing (0 = disable) */
	snap = 0,					/* Snap spacing (0 = disable) */
	tscale = 100,				/* Text scale (100 = 1:1) */
	xbase = 0,					/* Cursor display base COLUMN */
	ybase = 0;					/* Cursor display base ROW */
char tick = 1;					/* Position marker flag */

/* Misc. variables */
char pixel = SET_PIXEL,			/* Current video attribute for drawing */
	cursor_flag = 0,			/* Indicate cursor is on-screen */
	edit = -1,					/* Indicates EDIT mode */
	font[4608],					/* Text font storage */
	dfile[65],					/* Drawing file name */
	ffile[65],					/* Font file name */
	ifile[65],					/* Insert file name */
	vmode;						/* Saved video mode */

unsigned mousex = -1, mousey = -1;	/* Cursor X/Y position */

/* 
 * Sine table for drawing ARC's
 */
unsigned sine[] = {
	    0,  1608,  3216,  4821,  6424,  8022,  9616, 11204,
	12785, 14359, 15924, 17479, 19024, 20557, 22078, 23586,
	25079, 26557, 28020, 29465, 30893, 32302, 33692, 35062,
	36410, 37736, 39040, 40320, 41575, 42806, 44011, 45190,
	46341, 47464, 48559, 49624, 50660, 51665, 52639, 53581,
	54491, 55368, 56212, 57022, 57797, 58538, 59243, 59913,
	60547, 61144, 61705, 62228, 62714, 63162, 63571, 63943,
	64276, 64571, 64826, 65043, 65220, 65358, 65457, 65516 };

extern FILE *get_file();	/* Prototype for FILE function */

/*
 * Main program - process commands at the highest level
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	int i, j, select;
	char *ptr;
	FILE *fp;

	/* Set default file names */
	concat(dfile, "MICROCAD.DWG");
	concat(ffile, "MICROCAD.FNT");
	*ifile = 0;

	/* Parse command line arguments */
	for(i=1; i < argc; ++i) {
		ptr = argv[i];
		switch((toupper(*ptr++) << 8) | toupper(*ptr++)) {
			case 'F=' :		/* Specify alternate FONT file */
				concat(ffile, ptr, ".FNT");
				break;
			case '/D' :		/* Display only mode */
			case '-D' :
				edit = 0;
				break;
			case '?' << 8:	/* Command line help */
			case '/?' :
			case '-?' :
				abort("\nUse: MICROCAD [drawing file] [F=font file] [/Display]\n\nCopyright 1992-1993 Dave Dunfield\nAll rights reserved.\n");
			default:		/* Assume drawing filename */
				concat(dfile, argv[i], ".DWG"); } }


	/* Read in "font" file */
	fp = fopen(ffile, "qrbv");
	fread(font, sizeof(font), fp);
	fclose(fp);

	/* Read in the drawing file */
	if(fp = fopen(dfile, "rbv")) {
		fread(&grid, PBYTES, fp);
		dtop = fread(drawing, sizeof(drawing), fp);
		fclose(fp); }

	/* Zero unused portion of drawing file */
	zero_drawing(dtop);

	/* Initialize video adapter, and test for VGA */
	if(!init_video())
		abort("VGA required!");

	/* If not edit... Insure grid & tick off, display & exit */
	if(!edit) {
		grid = 0;
		tick = 1;
		redraw();
		get_key();
		video_mode(vmode);
		return; }

	/* Initialize mouse */
	if(!init_mouse())
		abort("MOUSE required!");

	/* Clear screen & draw current drawing */
	redraw();

	if(!fp) {
		message();
		printf("New drawing: '%s'", dfile); }

	/* Process commands */
	for(;;) {
		if((i=mouse_status()) & MOUSE_LEFT) {	/* Prompt for new command */
			message();
			printf("A)rc B)ox C)ircle D)up E)rase F)unc L)ine M)ove R)edraw S)etup T)ext U)ndo");
			select = get_key(); }
		else if(j = test_key())
			select = j;
		else if(!(i & MOUSE_RIGHT))				/* Repeat last command */
			continue;
		/* Perform command operation */
		switch(toupper(select)) {
			case 'A' :		/* Draw an ARC */
				draw_arc();
				break;
			case 'B' :		/* Draw BOX */
				draw_box();
				break;
			case 'C' :		/* Draw CIRCLE */
				draw_circle();
				break;
			case 'D' :		/* Duplicate object */
				copy();
				break;
			case 'E' :		/* Erase object */
				erase();
				break;
			case 'F' :		/* Function menu */
				function();
				break;
			case 'L' :		/* Draw LINE's */
				draw_line();
				break;
			case 'M' :		/* Move object */
				move();
				break;
			case 'R' :		/* Redraw the drawing */
				redraw();
				break;
			case 'T' :		/* Text entry */
				draw_text();
				break;
			case 'U' :		/* Erase last object from list */
				undo();
				break;
			case 'S' :		/* Settings */
				message();
				printf("B)ase-markers C)ursor-base G)rid S)nap T)ext-scale");
				switch(toupper(get_key())) {
					case 'B' :		/* Basepoint markers */
						tick = tick ? 0 : 1;
						redraw();
						continue;
					case 'C' :		/* Cursor base address */
						i = xbase;
						j = ybase;
						xbase = ybase = 0;
						if(!wait_for_left("Select cursor base point")) {
							xbase = mousex;
							ybase = mousey;
							goto exit; }
						xbase = i;
						ybase = j;
						continue;
					case 'G' :		/* Set/Remove GRID */
						grid = get_value("Grid spacing (0 to remove)?");
						redraw();
						continue;
					case 'S' :		/* Set/Remove SNAP */
						snap = get_value("Snap spacing (0 to remove)?");
						goto exit;
					case 'T' :		/* Text scale */
						tscale = get_value("Text scale (100 = 1:1)?");
					exit:
						message();
						continue; }
			default:
				message(); 
				printf("Unknown command!"); } }
}

/*
 * Perform misc. functions not on main menu.
 */
function()
{
	FILE *fp;
	
	message();
	printf("F)ont I)nsert L)oad Q)uit S)ave");
	switch(toupper(get_key())) {
		case 'I' :		/* Insert a drawing */
			insert();
			return;
		case 'F' :		/* Load alternate FONT file */
			if(fp = get_file("font", ffile, ".FNT", "rbv")) {
				fread(font, sizeof(font), fp);
				fclose(fp);
				redraw(); }
			return;
		case 'L' :		/* Load drawing file */
			if(fp = get_file("drawing", dfile, ".DWG", "rbv")) {
				fread(&grid, PBYTES, fp);
				zero_drawing(fread(drawing, sizeof(drawing), fp));
				fclose(fp);
				redraw(); }
			return;
		case 'S' :		/* Save drawing file */
			if(fp = get_file("drawing", dfile, ".DWG", "wbv")) {
				fwrite(&grid, PBYTES, fp);
				if(dtop)
					fwrite(drawing, dtop, fp);
				fclose(fp);
				message(); }
			return;
		case 'Q' :		/* Quit */
			video_mode(vmode);
			exit(0); }

		message();
		printf("Unknown command!");
}

/*
 * Insert another drawing into this one as a GROUP
 */
insert()
{
	unsigned i, dsave, base, size, minx, miny, x, y;
	char buffer[PBYTES];
	FILE *fp;

	/* Get filename & insertion point */
	if(!(fp = get_file("drawing", ifile, ".DWG", "rbv")))
		return;

	dsave = dtop;
	drawing[dtop++] = GROUP;
	draw(0);	/* Reserve 'X' */
	draw(0);	/* Reserve 'Y' */
	draw(0);	/* Reserve group size */

	/* Append the drawing objects */
	fread(buffer, PBYTES, fp);
	size = fread(&drawing[base = dpos = dtop], sizeof(drawing) - dtop, fp);
	fclose(fp);

	/* Locate base point of group */
	minx = miny = 32767;
	while(drawing[dpos]) {
		i = dpos++;
		minx = min(x = dvalue(), minx);
		miny = min(y = dvalue(), miny);
		dpos = i;
		skip_object(); }
	
	/* Reposition new drawing objects to relative offset from group base */
	dpos = base;
	while(x = drawing[i = dpos]) {
		dtop = ++dpos;
		draw(dvalue() - minx);
		draw(dvalue() - miny);
		/* Convert any absolute copy objects in the group relative copy
         * objects, so that group is entirely position independant. */
		if(x == ACOPY) {
			drawing[i] = RCOPY;
			draw((dvalue()+base) - i); }
		dpos = i;
		skip_object(); }

	/* Origin group at zero, and update size */
	dtop = dsave + 1;
	draw(0);
	draw(0);
	draw(size);

	/* Get selection point */
	message();
	printf("INSERT: Selection insertion point");
	pixel = FLIP_PIXEL;
newbox:
	dpos = dsave;
	draw_object(x = mousex, y=mousey);
	while(!((i = mouse_status()) & MOUSE_LEFT)) {
		if(i & MOUSE_RIGHT) {
			dpos = dsave;
			draw_object(x, y);
			zero_drawing(dsave);
			message();
			return; }
		if((x != mousex) || (y != mousey)) {
			dpos = dsave;
			draw_object(x, y);
			goto newbox; } }

	/* Fix up pointers and redraw */
	dpos = dsave;
	pixel = SET_PIXEL;
	dtop = dsave + 1;
	draw(x);
	draw(y);
	dtop = base + size;
	redraw();
}

/*
 * Position and draw one or more lines.
 */
draw_line()
{
	int x, y, x1, y1, s;
	if(wait_for_left("LINE: From point?"))
		return;
	gotoxy(30, 0); printf(" To point?");
	x = mousex;
	y = mousey;
again:
	pixel = FLIP_PIXEL;
newline:
	line(x, y, x1 = mousex, y1 = mousey);
	while(!((s = mouse_status()) & MOUSE_LEFT)) {
		if(s & MOUSE_RIGHT) {
			line(x, y, x1, y1);
			pixel = SET_PIXEL;
			message();
			return; }
		if((mousex != x1) || (mousey != y1)) {
			line(x, y, x1, y1);
			goto newline; } }
	pixel = SET_PIXEL;
	drawing[dpos = dtop++] = LINE;
	draw(x);
	draw(y);
	draw(x1 - x);
	draw(y1 - y);
	draw_object(0, 0);
	x = x1;
	y = y1;
	goto again;
}

/*
 * Position and draw a box
 */
draw_box()
{
	int x, y, x1, y1, s;
	if(wait_for_left("BOX: First corner?"))
		return;
	gotoxy(30, 0); printf(" Second corner?");
	x = mousex;
	y = mousey;
	pixel = FLIP_PIXEL;
newbox:
	box(x, y, x1 = mousex, y1 = mousey);
	while(!((s = mouse_status()) & MOUSE_LEFT)) {
		if(s & MOUSE_RIGHT) {
			box(x, y, x1, y1);
			message();
			return; }
		if((mousex != x1) || (mousey != y1)) {
			box(x, y, x1, y1);
			goto newbox; } }
	pixel = SET_PIXEL;
	message();
	drawing[dpos = dtop++] = BOX;
	draw(x);
	draw(y);
	draw(x1 - x);
	draw(y1 - y);
	draw_object(0, 0);
}

/*
 * Position and draw a circle
 */
draw_circle()
{
	int x, y, r, r1, a, b, s;
	if(wait_for_left("CIRCLE: Select center point"))
		return;
	x = mousex;
	y = mousey;
	pixel = FLIP_PIXEL;
	r = 1;
newcircle:
	circle(x, y, r);
	while(!((s = mouse_status()) & MOUSE_LEFT)) {
		if(s & MOUSE_RIGHT) {
			circle(x, y, r);
			message();
			return; }
		a = abs(mousex - x);
		b = abs(mousey - y);
		if((r1 = sqrt((a*a) + (b*b))) != r) {
			gotoxy(40, 0); printf("Radius=%-3u", r1);
			circle(x, y, r);
			r = r1;
			goto newcircle; } }
	pixel = SET_PIXEL;
	message();
	drawing[dpos = dtop++] = CIRCLE;
	draw(x);
	draw(y);
	draw(r);
	draw_object(0, 0);
}

/*
 * Position and draw an arc
 */
draw_arc()
{
	int x, y, r, v1, v2, x1, y1, s;

	if(wait_for_left("ARC: Select center point"))
		return;
	x = mousex;
	y = mousey;
	if(wait_for_left("ARC: Select starting point"))
		return;
	x1 = abs(mousex - x);
	y1 = abs(mousey - y);
	r = sqrt((x1*x1) + (y1*y1));
	v1 = find_vector(x, y, r, mousex, mousey);
	message();
	printf("ARC: Select ending point");
	pixel = FLIP_PIXEL;
newarc:
	v2 = find_vector(x, y, r, x1=mousex, y1=mousey);
	arc(x, y, r, v1, v2);
	while(!((s = mouse_status()) & MOUSE_LEFT)) {
		if(s & MOUSE_RIGHT) {
			arc(x, y, r, v1, v2);
			message();
			return; }
		if((mousex != x1) || (mousey != y1)) {
			arc(x, y, r, v1, v2);
			goto newarc; } }
	pixel = SET_PIXEL;
	message();
	drawing[dpos = dtop++] = ARC;
	draw(x);
	draw(y);
	draw(r);
	draw(v1);
	draw(v2);
	draw_object(0, 0);
}

/*
 * Position and draw a text string
 */
draw_text()
{
	unsigned i, x, y;
	char buffer[80], *ptr;

	message();
	printf("String?");
	fgets(ptr = buffer, sizeof(buffer)-1, stdin);
	message();
	if(*buffer) {
		printf("TEXT: Select position");
		pixel = FLIP_PIXEL;
	newtext:
		text(buffer, x = mousex, y = mousey, tscale);
		while(!((i = mouse_status()) & MOUSE_LEFT)) {
			if(i & MOUSE_RIGHT) {					/* Cancel */
				text(buffer, x, y, tscale);
				pixel = SET_PIXEL;
				message();
				return; }
			if((x != mousex) || (y != mousey)) {	/* Position changed */
				text(buffer, x, y, tscale);
				goto newtext; } }

		pixel = SET_PIXEL;
		message();
		drawing[dpos = dtop++] = TEXT;
		draw(mousex);
		draw(mousey);
		draw(tscale);
		do
			drawing[dtop++] = *ptr;
		while(*ptr++);
		draw_object(0, 0); }
}

/*
 * Draw the graphic cursor on the screen at mouse position
 */
draw_cursor()
{
	char s;

	s = pixel;
	pixel = FLIP_PIXEL;
	line(max(0, mousex-3), mousey, min(HORIZONTAL-1,mousex+3), mousey);
	line(mousex, max(0,mousey-3), mousex, min(VERTICAL-1,mousey+3));
	pixel = s;
}

/*
 * Initialize the screen (clearing it), and display the drawing.
 */
redraw()
{
	int x, y;

	video_mode(0x11);
	dpos = cursor_flag = 0;
	pixel = SET_PIXEL;
	while(!draw_object(0, 0));
	if(grid) {
		for(y = 0; y < VERTICAL; y += grid)
			for(x=0; x < HORIZONTAL; x += grid)
				set_pixel(x, y); }
}

/*
 * Remove the last operation from the drawing list, and redraw
 * it in "off" pixels, effectivly undoing the last operation.
 */
undo()
{
	unsigned dlast;
	dpos = dlast = 0;
	while(drawing[dpos]) {
		dlast = dpos;
		if(skip_object())
			return; }
	dpos = dlast;
	pixel = CLEAR_PIXEL;
	draw_object(0, 0);
	pixel = SET_PIXEL;
	zero_drawing(dlast);
	message();
}

/*
 * Erase an object from the drawing file
 */
erase()
{
	unsigned e, d, i, x, y;

	if(select_object("ERASE")) {
		d = dtop;
		e = dpos;		/* Address of object to erase */

		/* Scan list, for copies of the object we are about to delete.
         * If found, re-position this object to the address of its copy,
 		 * and delete the copy object instead. */
		while(i = drawing[dpos]) {
			if(i == ACOPY) {
				i = dpos++;
				x = dvalue();
				y = dvalue();
				if(dvalue() == e) {	/* Move copy into object */
					dtop = e + 1;
					draw(x);
					draw(y);
					e = i; }
				dpos = i; }
			skip_object(); }

		/* Scan list again, for copies referencing objects higher than the
		 * one we finally decided to delete, and adjust reference offset. */
		dpos = e;
		skip_object();
		x = dpos - e;
		while(i = drawing[dpos]) {
			if(i == ACOPY) {
				i = dpos;
				dpos += 5;
				if((y = dvalue()) > e) {
					dtop = dpos - 2;
					draw(y - x); }
				dpos = i; }
			skip_object(); }

	/* Remove the selected entry */
		dpos = e;
		skip_object();
		while(dpos < d)
			drawing[e++] = drawing[dpos++];
		zero_drawing(e); }
}

/*
 * Moves an object to a new location
 */
move()
{
	unsigned psave, i, x, y, sx, sy;
	if(select_object("MOVE")) {
		/* Save position in file */
		psave = dpos;
		/* Save old position in case we cancel */
		++dpos;
		sx = dvalue();
		sy = dvalue();
		message();
		printf("MOVE: Select new position");
		pixel = FLIP_PIXEL;
newmove:
		dpos = psave;
		draw_object((x = mousex) - sx, (y = mousey) - sy);
		while(!((i = mouse_status()) & MOUSE_LEFT)) {
			if(i & MOUSE_RIGHT) {					/* Cancel */
				dpos = psave;
				draw_object(x - sx, y - sy);
				goto skipmove; }
			if((x != mousex) || (y != mousey)) {	/* Position changed */
				dpos = psave;
				draw_object(x - sx, y - sy);
				goto newmove; } }

		/* Update object with new location */
		i = dtop;
		dtop = psave + 1;
		draw(x);
		draw(y);
		dtop = i;

skipmove:
		/* Draw the object in its final resting place */
		pixel = SET_PIXEL;
		message();
		dpos = psave;
		draw_object(0, 0); }
}

/*
 * Copies an object to a new location
 */
copy()
{
	unsigned psave, i, x, y, sx, sy;

	if(select_object("DUP")) {
		psave = dpos;
		++dpos;
		sx = dvalue();
		sy = dvalue();
		pixel = SET_PIXEL;
		dpos = psave;
		draw_object(0, 0);
		pixel = FLIP_PIXEL;
		message();
		printf("DUP: Select new position");
	newcopy:
		dpos = psave;
		draw_object((x = mousex) - sx, (y = mousey) - sy);
		while(!((i = mouse_status()) & MOUSE_LEFT)) {
			if(i & MOUSE_RIGHT) {
				dpos = psave;
				draw_object(x - sx, y - sy);
				message();
				return; }
			if((x != mousex) || (y != mousey)) {
				dpos = psave;
				draw_object(x - sx, y - sy);
				goto newcopy; } }

		/* Append a copy object pointing to original */
		drawing[dpos = dtop++] = ACOPY;
		draw(x);
		draw(y);
		draw(psave);

		message();
		pixel = SET_PIXEL;
		draw_object(0, 0); }
}

/*
 * Draws an object from the drawing file
 */
draw_object(xoffset, yoffset)
	int xoffset, yoffset;
{
	int x, y, i, j, k, l;
	char buffer[80], *ptr;

	if(cursor_flag)
		mouse_status();

	if(!(i = drawing[dpos++]))
		return -1;
	x = dvalue() + xoffset;
	y = dvalue() + yoffset;
	if(!tick++)
		box(max(0, x-1), max(0, y-1), min(HORIZONTAL-1, x+1), min(VERTICAL-1, y+1));

	switch(i) {
		case LINE :
			line(x, y, x+dvalue(), y+dvalue());
			break;
		case BOX :
			box(x, y, x+dvalue(), y+dvalue());
			break;
		case CIRCLE :
			circle(x, y, dvalue());
			break;
		case TEXT :
			i = dvalue();
			ptr = buffer;
			do
				*ptr++ = j = drawing[dpos++];
			while(j);
			text(buffer, x, y, i);
			break;
		case ARC:
			arc(x, y, dvalue(), dvalue(), dvalue());
			break;
		case GROUP :
			i = dvalue();
			j = dpos;
			while((dpos - j) < i)
				draw_object(x, y);
			break;
		case RCOPY :
			i = dpos - 5;
			i += dvalue();
			goto gocopy;
		case ACOPY :
			i = dvalue();
		gocopy:
			j = dpos;
			dpos = i+1;
			k = dvalue();
			l = dvalue();
			dpos = i;
			draw_object(x - k, y - l);
			dpos = j;
			break;
		default:
			message();
			printf("Corrupt drawing file!");
			zero_drawing(dpos -= 5);
			return -1; }
	--tick;
	return 0;
}

/*
 * Skip to next object in the drawing file
 */
skip_object()
{
	unsigned i;

	switch(drawing[dpos++]) {
		case LINE :
		case BOX :
			dpos += 8;
			break;
		case CIRCLE :
		case ACOPY :
		case RCOPY :
			dpos += 6;
			break;
		case TEXT :
			dpos += 6;
			while(drawing[dpos++]);
			break;
		case ARC :
			dpos += 10;
			break;
		case GROUP :
			dvalue();
			dvalue();
			i = dvalue();
			dpos += i;
			break;
		default :
			message();
			printf("Corrupt drawing file!");
			zero_drawing(--dpos);
		case 0 :
			return -1; }
	return 0;
}

/*
 * Selects an object with the cursor
 */
select_object(name)
	char *name;
{
	int x, y, i, b;
	char flag;

	flag = -1;
	for(;;) {
		if(flag) {
			message();
			printf("%s: Select object", name);
			flag = 0; }
		if((b = mouse_status()) & MOUSE_RIGHT) {
			message();
			return 0; }
		x = mousex;
		y = mousey;
		dpos = 0;
		while(drawing[i = dpos++]) {
			if((dvalue() == x) && (dvalue() == y)) {
				if(cursor_flag) {
					draw_cursor();
					cursor_flag = 0; }
				dpos = i;
				pixel = CLEAR_PIXEL;
				draw_object(0, 0);
				pixel = SET_PIXEL;
				if(wait_for_left("LEFT to accept, RIGHT to cancel")) {
					dpos = i;
					draw_object(0, 0);
					flag = -1;
					continue; }
				dpos = i;
				message();
				return -1; }
			dpos = i;
			if(skip_object())
				break; } }
}

/*
 * Find ARC vector pointing toward a certain point.
 *
 * Calculate the location of the point on the arc produced by each
 * vector, and record the vector which brings us closest to the
 * desired point.
 */
find_vector(x, y, r, sx, sy)
	int x, y, r, sx, sy;
{
	int rs, i, j, ax, x1, y1, x2, y2, v, v1;
	unsigned d, d1;

	rs = r*r;

	d1 = -1;
	for(v=0; v < (ARC_RES*4); ++v) {
		j = (ARC_RES-1) - (i = v & (ARC_RES-1));
		switch(v & (ARC_RES*3)) {
			case ARC_RES*0 :		/* Quadrant one */
				x1 = x + (ax = scale(r, sine[i], -1));
				y1 = y - sqrt(rs - (ax*ax));
				break;
			case ARC_RES*1 :		/* Quadrant two */
				x1 = x + (ax = scale(r, sine[j], -1));
				y1 = y + sqrt(rs - (ax*ax));
				break;
			case ARC_RES*2 :		/* Quadrant three */
				x1 = x - (ax = scale(r, sine[i], -1));
				y1 = y + sqrt(rs - (ax*ax));
				break;
			case ARC_RES*3 :		/* Quadrant four */
				x1 = x - (ax = scale(r, sine[j], -1));
				y1 = y - sqrt(rs - (ax*ax)); }
		x2 = abs(x1 - sx);
		y2 = abs(y1 - sy);
		if((d = sqrt((x2*x2) + (y2*y2))) < d1) {
			v1 = v;
			d1 = d; } }
	return v1;
}

/*
 * Draw a line from point (x1, y1) to (x2, y2)
 */
line(x1, y1, x2, y2)
	int x1, y1, x2, y2;
{
	int i, w, h;
	/* If 'X' is greater, increment through 'X' coordinate */
	if((w = abs(x1 - x2)) >= (h = abs(y1 - y2))) {
		if(x1 > x2) {
			i = x1;
			x1 = x2;
			x2 = i;
			i = y1;
			y1 = y2;
			y2 = i; }
		if(y1 < y2) {
			for(i=0; i < w; ++i)
				set_pixel(x1+i, y1+scale(i, h, w)); }
		else {
			for(i=0; i < w; ++i)
				set_pixel(x1+i, y1-scale(i, h, w)); } }
	/* If 'Y' is greater, increment through 'Y' coordinate */
	else {
		if(y1 > y2) {
			i = x1;
			x1 = x2;
			x2 = i;
			i = y1;
			y1 = y2;
			y2 = i; }
		if(x1 < x2) {
			for(i=0; i < h; ++i)
				set_pixel(x1+scale(i, w, h), y1+i); }
		else {
			for(i=0; i < h; ++i)
				set_pixel(x1-scale(i, w, h), y1+i); } }

	set_pixel(x2, y2);
}

/*
 * Draw a box with opposite corners (x1, y1) to (x2, y2)
 */
box(x1, y1, x2, y2)
	int x1, y1, x2, y2;
{
	line(x1, y1, x2, y1);		/* Top */
	line(x1, y1+1, x1, y2-1);	/* Left side */
	line(x2, y1+1, x2, y2-1);	/* Right side */
	line(x1, y2, x2, y2);		/* Bottom */
}

/*
 * Draw a circle about point (x, y) of radius (r)
 *
 * For speed, we only calculate 1 quadrant of the circle,
 * and mirror it into the other three quadrants.
 */
circle(x, y, r)
	int x, y, r;
{
	int i, j, k, rs, lj;

	rs = (lj = r)*r;
	for(i=0; i <= r; ++i) {
		j = k = sqrt(rs - (i*i));
		do {
			set_pixel(x+i, y+j);
			set_pixel(x+i, y-j);
			set_pixel(x-i, y+j);
			set_pixel(x-i, y-j); }
		while(++j < lj);
		lj = k; }
}

/*
 * Draw an arc centered at (x, y), radius r at vectors v1, v2
 */
arc(x, y, r, v1, v2)
	int x, y, r;
	unsigned char v1, v2;
{
	int rs, i, j, ax, x1, y1, x2, y2;

	x2 = -1;
	rs = r*r;

	do {
		j = (ARC_RES-1) - (i = v1 & (ARC_RES-1));
		switch(v1 & (ARC_RES*3)) {
			case ARC_RES*0 :	/* Quadrant one */
				x1 = x + (ax = scale(r, sine[i], -1));
				y1 = y - sqrt(rs - (ax*ax));
				break;
			case ARC_RES*1 :	/* Quadrant two */
				x1 = x + (ax = scale(r, sine[j], -1));
				y1 = y + sqrt(rs - (ax*ax));
				break;
			case ARC_RES*2 :	/* Quadrant three */
				x1 = x - (ax = scale(r, sine[i], -1));
				y1 = y + sqrt(rs - (ax*ax));
				break;
			case ARC_RES*3 :	/* Quadrant four */
				x1 = x - (ax = scale(r, sine[j], -1));
				y1 = y - sqrt(rs - (ax*ax)); }
		if(x2 != -1)
			line(x2, y2, x1, y1);
		x2 = x1;
		y2 = y1; }
	while(v1++ != v2);
}

/*
 * Draw a text string on the screen at specified scale
 */
text(string, x, y, s)
	char *string;
	int x, y, s;
{
	unsigned i, j, b;
	unsigned char *ptr;

	y -= scale(24, s, 100);
	while(*string) {
		ptr = &font[(*string++ - ' ') * 48];

		for(i=0; i < 24; ++i) {
			b = (*ptr++ << 8) | *ptr++;
			for(j=0; j < 16; ++j) {
				if(b & 0x8000)
					set_pixel(x+scale(j,s,100), y+scale(i,s,100));
				b <<= 1; } }
		x += scale(18, s, 100); }
}

/*
 * Clear message area at top of screen & position text cursor there
 */
message()
{
	int i;
	gotoxy(0, 0);
	for(i=0; i < 80; ++i)
		putc(' ', stdout);
	gotoxy(0, 0);
}

/*
 * Write title, and wait for mouse button. Return 0 if LEFT, -1 if RIGHT.
 */
wait_for_left(prompt)
	char *prompt;
{
	int s;
	message();
	printf(prompt);
	while(!((s = mouse_status()) & MOUSE_LEFT)) {
		if(s & MOUSE_RIGHT) {
			message();
			return -1; } }
	return 0;
}

/*
 * Write a 16 bit value to the drawing list
 */
draw(value)
	unsigned value;
{
	drawing[dtop++] = value >> 8;
	drawing[dtop++] = value & 255;
}

/*
 * Retrieve a 16 bit value from the drawing list
 */
dvalue()
{
	return (drawing[dpos++] << 8) | drawing[dpos++];
}

/*
 * Zero out the top of the drawing & set upper boundary
 */
zero_drawing(top)
	unsigned top;
{
	dtop = top;
	while(top < sizeof(drawing))
		drawing[top++] = 0;
}

/*
 * Prompt for, and get a numeric value from the user
 */
get_value(prompt)
	char *prompt;
{
	unsigned value;
	char buffer[51], *ptr;

	message();
	printf(prompt);
	fgets(ptr = buffer, sizeof(buffer)-1, stdin);
	while(isspace(*ptr))
		++ptr;
	value = 0;
	while(isdigit(*ptr))
		value = (value*10) + (*ptr++ - '0');
	message();
	return value;
}

/*
 * Get a filename & access it with specified mode
 */
FILE *get_file(prompt, name, ext, mode)
	char *prompt, *name, *ext, *mode;
{
	int i, dot;
	char buffer[65], *ptr;

	message();
	printf("Enter %s filename (%s)?", prompt, name);
	fgets(ptr = buffer, sizeof(buffer)-1, stdin);
	while(isspace(*ptr))
		++ptr;
	if(*ptr) {
		dot = -1;
		for(i = 0; *ptr; ++i)
			if((name[i] = *ptr++) == '.')
				dot = i;
		name[i] = 0;
		if(dot == -1)
			while(name[i++] = *ext++); }
	message();
	return fopen(name, mode);
}

/*
 * Scale a value by a fraction, using a 32 bit intermediate result,
 * and round up/down to nearest integer ressult.
 */
scale(value, mul, div) asm
{
		MOV		AX,8[BP]		; Get value
		MUL		WORD PTR 6[BP]	; Multiply to 32 bit product
		MOV		BX,4[BP]		; Get divisor
		DIV		BX				; Divide back to 16 bit result
		SHR		BX,1			; /2 for test
		JZ		scale1			; Special case (/1)
		INC		DX				; .5 rounds up
		SUB		BX,DX			; Set 'C' if remainder > half
		ADC		AX,0			; Increment result to scale
scale1:
}

/*
 * Initialize the video display
 */
init_video() asm
{
; First, determine current mode for later
		MOV		AH,0Fh			; Get video mode
		INT		10h				; Call BIOS
		MOV		DGRP:_vmode,AL	; Save video mode
; Check for VGA present
		MOV		AX,1A00h		; Get display code
		INT		10h				; Call BIOS
		CMP		AL,1Ah			; VGA supported?
		MOV		AL,-1			; Assume yes
		JZ		initv1			; It does exist
		XOR		AX,AX			; Report failure
initv1:
}

/*
 * Set active video mode & clear screen
 */
video_mode(mode) asm
{
		MOV		AL,4[BP]		; Get mode
		XOR		AH,AH			; Func 0 - set mode
		INT		10h				; Issue mode
}

/*
 * Initializes the mouse driver, returns with -1 if successful.
 */
init_mouse() asm
{
; Initialize & test for mouse
		XOR		AX,AX			; Init functions.
		INT		33h				; Call mouse driver
		AND		AX,AX			; Mouse present
		JZ		initm1			; No, skip it
; Set mouse limits (some drivers do not do it properly on reset)
		XOR		CX,CX			; Lower limit is zero
		MOV		DX,639			; Upper horizontal limit
		MOV		AX,7			; Set horizontal limit
		INT		33h				; Call mouse driver
		MOV		DX,479			; Upper vertical limit
		MOV		AX,8			; Set vertical limit
		INT		33h				; Call mouse driver
		MOV		AX,-1			; Indicate mouse ok
initm1:
}

/*
 * Set a graphic pixel, to the attribute previously set in 'pixel'
 */
set_pixel(x, y) asm
{
		MOV		DX,4[BP]		; Get Y coordinate
		CMP		DX,VERTICAL		; In range?
		JAE		noset			; No skip it
		MOV		CX,6[BP]		; Get X coordinate
		CMP		CX,HORIZONTAL	; In Range?
		JAE		noset			; No, skip it
		MOV		AH,0Ch			; Write pixel function
		MOV		AL,DGRP:_pixel	; Pixel attribute
		XOR		BH,BH			; Zero page
		INT		10h				; Call int 16
noset:
}

/*
 * Position the text cursor
 */
gotoxy(x, y) asm
{
		MOV		DH,4[BP]		; Get Y coordinate
		MOV		DL,6[BP]		; Get X coordinate
		XOR		BH,BH			; Zero page
		MOV		AH,02h			; Set cursor
		INT		10h				; Call BIOS
}

/*
 * Test for a key from the console
 */
test_key() asm
{
		MOV		AH,01h			; Test for key function
		INT		16h				; Ask BIOS
		JNZ		getk1			; Ready, get character
		XOR		AX,AX			; Zero result
}

/*
 * Get a key without echo or editing
 */
get_key() asm
{
getk1:	XOR		AH,AH			; Get key function
		INT		16h				; Ask BIOS
		XOR		AH,AH			; Zero HIGH byte
}

/*
 * Update mouse position and on-screen cursor. If any button is
 * activated, remove cursor, wait for button to be released, and
 * report it.
 */
mouse_status()
{
	int x, y, z;

	/* If no cursor on screen, draw one */
	if(!cursor_flag) {
newcursor:
		draw_cursor();
		cursor_flag = -1; }
	/* Get mouse position and button status */
	asm {
		MOV		AX,0003h	; Mouse status function
		INT		33h			; Call mouse driver
		MOV		-2[BP],BX	; Save buttons
		MOV		-4[BP],DX	; Save Y position
		MOV		-6[BP],CX	; Save X position
		}

	/* If snap enabled, force cursor alignment */
	if(snap) {
		x = (x / snap) * snap;
		y = (y / snap) * snap; }

	/* If cursor position changed, update cursor and display*/
	if((x != mousex) || (y != mousey)) {
		draw_cursor();
		gotoxy(68, 0);
		printf("X=%-3d Y=%-3d", (mousex = x) - xbase, (mousey = y) - ybase);
		goto newcursor; }

	/* If any buttons are activated, wait for release, remove cursor */
	if(z & (MOUSE_LEFT|MOUSE_RIGHT|MOUSE_CENTER)) {
		asm {
			mloop1:	MOV		AX,0003h		; Mouse status function
					INT		33h				; Call mouse driver
					AND		BL,07h			; Any buttons down?
					JNZ		mloop1			; Wait till clear
			}
		draw_cursor();
		cursor_flag = 0; }

	/* Pass back button status to caller */
	return z;
}
