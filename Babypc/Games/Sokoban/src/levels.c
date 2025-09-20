
// ****************************************************************************
//
//                                 Game levels
//
// ****************************************************************************

//   x = grass (empty)
//   # = wall
//   ! or | = next row
//   space or _ or - = floor
//   $ or b = box
//   * or B = box on mark
//   . = mark
//   @ or p = player
//   + or P = player on mark

#include "../include.h"

// selected level
int Level;		// current level 0..
const char* LevDef;	// current level definition
const char* LevSolve;	// current level solve

const char* const Levels[] = {

// ===== MiniCosmos collection, (c) Aymeric du Peloux

	// 1
	"xx#####x!"
	"###   #x!"
	"# $ # ##!"
	"# #  . #!"
	"#    # #!"
	"## #   #!"
	"x#@  ###!"
	"x#####xx",
	"",//"uuluuRldd3ruruullDDullddrRlddrruUluR",

	// 2
	"xx#####x!"
	"###   #x!"
	"# $ # ##!"
	"# #  . #!"
	"#    # #!"
	"##$#.  #!"
	"x#@  ###!"
	"x#####xx",
	"",//"rruu3luurlddrrurru2(ulldd)rru3rddlldlludrruuluullddrrurd",

	// 3
	"xx#####x!"
	"###   #x!"
	"# $ # ##!"
	"# #  . #!"
	"# .  # #!"
	"##$#.$ #!"
	"x#@  ###!"
	"x#####xx",
	"",//"rruu3luurldd3rddlludrr3urrddlruuluulldllddrddrruudd2(lluur)rdrdl",

	// 4
	"xxxx####!"
	"#####  #!"
	"#   $  #!"
	"#  .#  #!"
	"## ## ##!"
	"#      #!"
	"# @#   #!"
	"#  #####!"
	"####xxxx",
	"",//"4urRll3d3ruuruul3Duu3l3drrdrru3Lrr3u3lddDlddr3Ud3r3u4ldR",

	// 5
	"xxxx####!"
	"#####  #!"
	"#   $  #!"
	"# *.#  #!"
	"## ## ##!"
	"#      #!"
	"# @#   #!"
	"#  #####!"
	"####xxxx",
	"",//"3ururldldd3ruuruul3duulldllu3rll3drrdrru3lrruuruul3duu3l3dlddr3udrrdrru3lrr3u4ldrddlddr3u",

	// 6
	"xxxx####!"
	"#####  #!"
	"#   *  #!"
	"# *.#  #!"
	"## ## ##!"
	"# $    #!"
	"# @#   #!"
	"#  #####!"
	"####xxxx",
	"",//"luruururldlddrrdrru3lrruuruul3duulldllu3rll3dlddr3udrrdrru3lrruuruuld4ldrddlddr3ud3ruurul",

	// 7
	"x#####xx!"
	"x#   ##x!"
	"## #$ ##!"
	"# $    #!"
	"#. .#  #!"
	"### @ ##!"
	"xx# # #x!"
	"xx#   #x!"
	"xx#####x",
	"",//"lullu3rlluurrdrdllruullddldrurddrruru3lruullddrdlrdrrddlluu",

	// 8
	"x#####xx!"
	"x#   ##x!"
	"##.#$ ##!"
	"# $    #!"
	"#. .#$ #!"
	"### @ ##!"
	"xx# # #x!"
	"xx#   #x!"
	"xx#####x",
	"",//"luurrddllullu3rlluurrdrdllddrrurulluullddldr3urrddlruulldd3rddllddrr3uru3ldlu3r4dlluu",

	// 9
	"x#####xx!"
	"x#   #xx!"
	"##$# ###!"
	"#   $@ #!"
	"# #  # #!"
	"# #. . #!"
	"#   ####!"
	"#####xxx",
	"",//"rdd3luuruulldurr4drruulrdd3ldll3uruurr3durrddll4ullddrluurr3du3l3drruruuld",

	// 10
	"x#####xx!"
	"x#   #xx!"
	"##$# ###!"
	"#  .$@ #!"
	"# #  # #!"
	"# #..$ #!"
	"#   ####!"
	"#####xxx",
	"",//rddllulur3l3drrur4ulldurr4drruulrdd3ldll3uruurr3d3ullddrll3drruuddll3uruu2(rrdd)llrruulld",

	// 11
	"x#####xxx!"
	"##   ###x!"
	"# . .  #x!"
	"# # ## ##!"
	"#    $$@#!"
	"### #   #!"
	"xx#   ###!"
	"xx#####xx",
	"",//dlullrddll3udlluururrdrrdullulldrrlddrrdrru3lrddll3udlluururrdldd3ruul",

	// 12
	"x#####xxx!"
	"##   ###x!"
	"# . . .#x!"
	"# # ## ##!"
	"#    $$@#!"
	"### # $ #!"
	"xx#   ###!"
	"xx#####xx",
	"",//dlu3lddrrudll4ullddrluu5rddlddll3udlluu3rl4drruulrddll3udlluururrdldd4rdl3ulrdd3lddrrudlluurrdruu",

	// 13
	"xxxx####x!"
	"x####  #x!"
	"## $   #x!"
	"#  # #$#x!"
	"#.@.   ##!"
	"## # #  #!"
	"x#      #!"
	"x#  #####!"
	"x####xxxx",
	"",//rruurr3drd3luulluurr4dldl3urruulldld3ruurur4drd4ldluudrruulluu4rddll",

	// 14
	"xxxx####x!"
	"x####  #x!"
	"## $   #x!"
	"#  # #$#x!"
	"#.@..  ##!"
	"## # #  #!"
	"x#   $  #!"
	"x#  #####!"
	"x####xxxx",
	"",//4rddll4urrdd4luurr4dldl3u4rdrd3l4ulldld3rddldl3urruulldldrrluu3rur4drd4ldluudrruulluu4rddllrruulld",

	// 15
	"xxx####x!"
	"####  #x!"
	"# $   #x!"
	"#  .# ##!"
	"## #.  #!"
	"# @  $ #!"
	"#   ####!"
	"#####xxx",
	"",//uulurrl3drrurrd3ldl3urururddulldldd4ruluulldllu3rur3drd3ldluud3r3u4ldr",

	// 16
	"xxx####x!"
	"####  #x!"
	"# $ $ #x!"
	"#  .# ##!"
	"## #.  #!"
	"# @  $ #!"
	"#.  ####!"
	"#####xxx",
	"",//rrurrd3ldlluru3(ur)3dldlrurrdllr3ulldldduulu3rur3dldldluudrruruu4ldrddrrurrd3ldl3ulu3rurddu3l3d4rul",

	// 17
	"xxxx#####x!"
	"xxx##   ##!"
	"xx## .#  #!"
	"x##   @  #!"
	"##    #  #!"
	"#  $ #####!"
	"# * ##xxxx!"
	"#  ##xxxxx!"
	"####xxxxxx",
	"",//ld3ldlddrurudllururlddrurudllururrdluldrr3urrddlldldluldruurdru",

	// 18
	"xxxx#####x!"
	"xxx##   ##!"
	"xx## .#  #!"
	"x##   @  #!"
	"##  * #  #!"
	"#  $ #####!"
	"# * ##xxxx!"
	"#  ##xxxxx!"
	"####xxxxxx",
	"",//3ldldldd3(ru)dldllururu3r3ldlddrurudllurur2(rdluld)u3r3urrdrddlulluldrdlr3urrdrdllruulldlddrulddllur",

	// 19
	"####xxxx!"
	"#  ####x!"
	"#     #x!"
	"#     #x!"
	"### ###x!"
	"# $$  ##!"
	"# . .@ #!"
	"####   #!"
	"xxx#####",
	"",//ll3urul4dllurdr3ulluurdldrur3dldrrl3urrulldlluurdldrur3drdlrdrrul",

	// 20
	"####xxxx!"
	"#  ####x!"
	"#     #x!"
	"#     #x!"
	"### ###x!"
	"# $$$ ##!"
	"# ...@ #!"
	"####   #!"
	"xxx#####",
	"",//ll3urul4dllurdr3ulluurdldrurdrrull3dldrrdrrululldllurdrrl3ulluurdldrurdrrull3drdl3ulluurdldrur3drddrrul",

	// 21
	"#####xx!"
	"#   ###!"
	"#     #!"
	"##    #!"
	"####$##!"
	"#  $ ##!"
	"# @   #!"
	"###. .#!"
	"xx#####",
	"",//rr3uruldlluurdldrur3dldllurrddrrul3uruldlluurdldrur4dldruu3ldrurd",

	// 22
	"#####xx!"
	"#   ###!"
	"# .   #!"
	"## $  #!"
	"####$##!"
	"#  $ ##!"
	"# @   #!"
	"###. .#!"
	"xx#####",
	"",//rr3urullulldrdrluurdr3dldllurrddrrul3urullulldrdrluurdr4dlr4ulldrur3dlddruu3ldrurdr3urull",

	// 23
	"xxxxxx####x!"
	"#######  #x!"
	"#        #x!"
	"#  $ #.# #x!"
	"#  $## # ##!"
	"###   @   #!"
	"xx###  #  #!"
	"xxxx##.  ##!"
	"xxxxx#####x",
	"",//3ulldlullddrulu6rur4dllddrrurulddlluu3luulu5rur4dllddrrurullr3ull4d4u3l3drrdruu",

	// 24
	"xxxxxx####x!"
	"#######  #x!"
	"#        #x!"
	"#  $ #.# #x!"
	"# $$## # ##!"
	"### . @   #!"
	"xx###  #  #!"
	"xxxx##.  ##!"
	"xxxxx#####x",
	"",//3ulldlu3r3d3lud3r3u5lddrulu3rdlru3rur4dllddrrurullrddlluu3lululu6rur4dllddrruruldd"
//	"lluu3luulu5rur4dllddrrurul3ull4d4u3l3drrdru3l3u5r3d3lru",

	// 25
	"xxxxx####!"
	"xxx###  #!"
	"x### .. #!"
	"x# $$#  #!"
	"## # #@##!"
	"#       #!"
	"#   #   #!"
	"######  #!"
	"xxxxx####",
	"",//uulldduurr3d3ldlluruurldd3rdr3uruuldlldlldd4ruurul3d4luurrdulldd3rdr3u",

	// 26
	"xxxxx####!"
	"xxx###  #!"
	"x### .. #!"
	"x# $$#  #!"
	"## # #@##!"
	"#  *    #!"
	"#   #   #!"
	"######  #!"
	"xxxxx####",
	"",//d3ldllu4rdr3uruuldlldduurr3drddluu3ldlluruurldd3rdrruluuruuldlldlldd3rdrr"
//	"uluurul3d3ldllu4rdru4luurrdulldd3rdrru3lrruu",

	// 27
	"x####xxx!"
	"x#  ####!"
	"x#     #!"
	"x# #.  #!"
	"##*##$##!"
	"#      #!"
	"# # @  #!"
	"#    ###!"
	"######xx",
	"",//uruululldduurrdrddldd3luurrdrr3uru3lul3duu3r4dllurdruudldd3luurrl3u4rdl3dllurdr3uru3luldd",

	// 28
	"x####xxx!"
	"x#  ####!"
	"x#     #!"
	"x# #.  #!"
	"##*##$##!"
	"#    * #!"
	"# # @  #!"
	"#    ###!"
	"######xx",
	"",//rruluululldduurrdr3dllurdd3luur3urrdrddrdlld3luu3rll3urrdrru3lul3duu3r3dldd3luurldd3r"
//	"uurrdl4u3l3drr3ldd3ruruuddld3luur3u4rdlu3l3drrdr3uru3lulddu3r3dlddludlluu3r",

	// 29
	"########!"
	"#   #  #!"
	"#      #!"
	"## #.  #!"
	"#    ###!"
	"# # . #x!"
	"# $$# #x!"
	"###  @#x!"
	"xx#####x",
	"",//lluur3ullddlddrluurrdruuddrd2(dlluu)rrluurrddurruuldrdl",

	// 30
	"########!"
	"#   #  #!"
	"#      #!"
	"## #. .#!"
	"#    ###!"
	"# # * #x!"
	"# $$# #x!"
	"###  @#x!"
	"xx#####x",
	"",//lluuddrruul3ullddrdrrddlluuruurullulldrddlddrluuruurrdrru3luldrr3drddllulluurrll"
//	"ddrruruurullulld3r3dldlluurrdruurruuldrdlullulld3rddlludrrurruuldrdlullulld4rurd",

	// 31
	"#####xx!"
	"#   ##x!"
	"# #  ##!"
	"#. #$ #!"
	"#  @  #!"
	"#.##$##!"
	"#    #x!"
	"######x",
	"",//ll3ur3(rd)luulull5d3ruulluluur3(rd)ldd3l3u3d3ruulluld3ruululld",

	// 32
	"####xxx!"
	"#  ###x!"
	"#    ##!"
	"# .#$ #!"
	"## @  #!"
	"x#.#$##!"
	"x#   #x!"
	"x#####x",
	"",//luurrdrdluull4drruulrddll3uluurdrrdrdlddll3ulur4drruulrddll3uluur3drruulrddlluluurd",

	// 33
	"x#######x!"
	"x#  #  ##!"
	"## **$. #!"
	"#   #   #!"
	"#   @ ###!"
	"#  ####xx!"
	"####xxxxx",
	"",//ll3urdldd3ruruuldllulddlddrurudrruruuldllddrrudlluurrdd4lurd3ruulluld3rd2(dllu)rurlddru",

	// 34
	"x#######x!"
	"##  #  #x!"
	"#  **$.##!"
	"#   #   #!"
	"### @   #!"
	"xx####  #!"
	"xxxxx####",
	"",//rr3uldrdd3luluurdrrurddrddluludlluluurdrrddlludrruulldd4ruld3luurrurddu3ldllurdrdrruruldrrddluu",

	// 35
	"xx####xx!"
	"###  ###!"
	"#   *$ #!"
	"# #  #@#!"
	"# # *. #!"
	"#   ####!"
	"#####xxx",
	"",//dlluluurd3l3drru3ruu3l3rdd3lud3ruulldldr3uldrddldll3urrurddu3l3drruuddll3urr",

	// 36
	"#####xxx!"
	"#   ##xx!"
	"# #  ###!"
	"#   *$ #!"
	"###  #@#!"
	"xx# *. #!"
	"xx#  ###!"
	"xx####xx",
	"",//dlldl3ulluurrdrdldd3ruulldldr3ulduullddrrdrddl3ulluurrdrdduulullddrr",

	// 37
	"xx####x!"
	"x##  #x!"
	"##   ##!"
	"#  *$ #!"
	"# # #@#!"
	"#  *. #!"
	"###  ##!"
	"xx#  #x!"
	"xx####x",
	"",//dlluullddrluurrddrruullddlluururddurrddll4urdl3drruulrddlluuddrddluurruu4lddrrlluurrduruuld",

	// 38
	"xx####xx!"
	"xx#  ###!"
	"x## .  #!"
	"##@$$$ #!"
	"# . . ##!"
	"#   ###x!"
	"#  ##xxx!"
	"####xxxx",
	"",//d3rurulldrdlldllururrdlr3ulddu3rdldlulldrllddru",

	// 39
	"xx#####x!"
	"###   #x!"
	"#     #x!"
	"#  #.###!"
	"##@$$$ #!"
	"x#.#.# #!"
	"x#     #!"
	"x#  ####!"
	"x####xxx",
	"",//ddrr3u3dl2(luur)urrdlddlluururdll4d4ruu3l3rdd4lu2(urrd)d3ldlurruullulurrurrdlddlluururd",

	// 40
	"xx####x!"
	"x##  #x!"
	"## . ##!"
	"#@$$$ #!"
	"#. .# #!"
	"# #   #!"
	"#   ###!"
	"#####xx",
	"",//drrudllururdll3drrurruulluld3rddllulrdrruulluurdlldrddrruulrddlluulldru3rddlldllu",

// ===== XSokoban collection, Original Levels & extras, (c) Thinking Rabbit

	// 1
	"xxxx#####xxxxxxxxxx!"
	"xxxx#   #xxxxxxxxxx!"
	"xxxx#$  #xxxxxxxxxx!"
	"xx###  $##xxxxxxxxx!"
	"xx#  $ $ #xxxxxxxxx!"
	"### # ## #xxx######!"
	"#   # ## #####  ..#!"
	"# $  $          ..#!"
	"##### ### #@##  ..#!"
	"xxxx#     #########!"
	"xxxx#######xxxxxxxx!",
	"",//"u3l3ululldll3d11rurd12lulld13rdrrlu7l3ulull3duull3d11rurrld7l3ullul3duull3d11rdru7l3ull3urdduu"
//	"ll5duull3d13r8l3ullul3duull3d12rlur",

	// 2
	"############xx!"
	"#..  #     ###!"
	"#..  # $  $  #!"
	"#..  #$####  #!"
	"#..    @ ##  #!"
	"#..  # #  $ ##!"
	"###### ##$ $ #!"
	"xx# $  $ $ $ #!"
	"xx#    #     #!"
	"xx############!",
	"",//rdrr3drruuluuruullu4ldduu4rdrrddld3lu6ldl3udd5r3urrdluldduu4rdrrddld3lu6ldluud5r3u4rd3lulddu6rdd"
//	"ld3lu6l4r4d3lurrdr4urrd3rdrdd4lulldllurdr3uddrrdrru4lrrd4ruululldulu5l3r4dllurdr3uddrrdrruluulu4l"
//	"dlu5rdrdd3ldllurdr3udd3ruulu4lullrd5rd3rdrd6ldllurdr3udd3ruulu4ldlluuddru5rdrrdrd5ldllurdr3udd3ru"
//	"ulu5l5rd3ruuru4lulldduurrd3r3d3lu4luld3r3urrdlulddu5r3d3lu4ldl3urul",

	// 3
	"xxxxxxxx########x!"
	"xxxxxxxx#     @#x!"
	"xxxxxxxx# $#$ ##x!"
	"xxxxxxxx# $  $#xx!"
	"xxxxxxxx##$ $ #xx!"
	"######### $ # ###!"
	"#....  ## $  $  #!"
	"##...    $  $   #!"
	"#....  ##########!"
	"########xxxxxxxxx!",
	"",//lddll4d8l8rurrdrrulld9l7r4urr3drd9l4ruurdrd5ldlu8r6u4lddr3drd5lu4l3rd5r3urdduurur3dld6ld4l3ru4ru"
//	"urrdrd6lu3lrrd4ru5rd8ld3lrru5r4uluur5drd5lullrd5r6urrddrdlul3drd6ldllru5r4uluur5drd5luldrdl",

	// 4
	"xxxxxxxxxxx########!"
	"xxxxxxxxxxx#  ....#!"
	"############  ....#!"
	"#    #  $ $   ....#!"
	"# $$$#$  $ #  ....#!"
	"#  $     $ #  ....#!"
	"# $$ #$ $ $########!"
	"#  $ #     #xxxxxxx!"
	"## #########xxxxxxx!"
	"#    #    ##xxxxxxx!"
	"#     $   ##xxxxxxx!"
	"#  $$#$$  @#xxxxxxx!"
	"#    #    ##xxxxxxx!"
	"###########xxxxxxxx!",
	"",//llul4Luluuluur5RurRu3RurD4ldllu8R8l4drruUUlu5R4lddrUlu3Rd4RUUllDDlU4l4dr3Ulu6R8l3dRUUlu7R4l4d"
//	"l3Ulu5R6ldd6lddrUlu7R6l4dlddr5Ulu6RdrUUlu5Ru4RDrDuUllDDR3lU3l3dlUUlu4Ru4R3lDRR8ldd3lddLUlu5Rd"
//	"dlUrrUl3Ldllu6RUlu7R4l3dlUUlu5R7lDrd3Ldllu6R6ld3Dlddr5Ulu6R5l7drrULdl6Ulu5RdrUUlu6Rd3RURDllUU"
//	"RllD3l3dlUUlu7R6l3dlU6l5dr3Rurrd5Ldl5Ulu7RUlu7R4l3dlUUlu5R7ldd4l5d6rddlUru5Ldl5Ulu6R5l5d5rddl"
//	"Uru4Ldl5Ulu5RdrUUlu6Rd3RUUllD4l3dlUUd7luurDld7R6luurDld5Ruu5RdRRlU3l3dlUUlu4RuRRlD4l3dlU5luur"
//	"Dld5RUlu5RdRUR4l3dlUUlu5RluR",

	// 5
	"xxxxxxxx#####xxxx!"
	"xxxxxxxx#   #####!"
	"xxxxxxxx# #$##  #!"
	"xxxxxxxx#     $ #!"
	"######### ###   #!"
	"#....  ## $  $###!"
	"#....    $ $$ ##x!"
	"#....  ##$  $ @#x!"
	"#########  $  ##x!"
	"xxxxxxxx# $ $  #x!"
	"xxxxxxxx### ## #x!"
	"xxxxxxxxxx#    #x!"
	"xxxxxxxxxx######x!",
	"",//luulld9l8rddluru7l8rurrd9l9rddluru8l7rddluru6ldlu8r3dlrrdd3l4uru6lu4l3rd4r3u4rdd3l3ruu4luur"
//	"rdull4drd4ld4l3ru4ru4ruu3lruull4drd4lu3lrrd4ru4rurru5lruull4drd4ld3lrru4rd4r3uru4lruull4drd"
//	"4lullrd4r3druuru5ldllru7r3drrdd3l4uru6luldrdl",

	// 6
	"######xx###x!"
	"#..  #x##@##!"
	"#..  ###   #!"
	"#..     $$ #!"
	"#..  # # $ #!"
	"#..### # $ #!"
	"#### $ #$  #!"
	"xxx#  $# $ #!"
	"xxx# $  $  #!"
	"xxx#  ##   #!"
	"xxx#########!",
	"",//dr4dlDr3uLDr3ulD5LulD7rddlUru6LdLLrU6rddlUru6L7r4dll3Uru5LuLLDDulUdDrrU7r6dllUll4U4drr3Uruul"
//	"l5L7rddlUru6L3r4dllddrUluRdr4U4drrur3Ulul3LuLLrD5rurrd6LdLLUUrD3r4dlluRdr3U4drrdr5Ulul4L5rur"
//	"rd6LdLUruL",

	// 7
	"xxxxxxx#####x!"
	"x#######   ##!"
	"## # @## $$ #!"
	"#    $      #!"
	"#  $  ###   #!"
	"### #####$###!"
	"# $  ### ..#x!"
	"# $ $ $ ...#x!"
	"#    ###...#x!"
	"# $$ #x#...#x!"
	"#  ###x#####x!"
	"####xxxxxxxxx!",
	"",//ld5r4d4u7ldr4dllurl3druurrdl3urd5rurddl5uluur6dldru6lddrulu4r3luluulu6rur6duuld4ld3lu"
//	"urld8r3urruulul4duu6l3dld6rurd3urrulul3duu4ldlul3dld6r7l3druulu6r",

	// 8
	"xx####xxxxxxxxxx!"
	"xx#  ###########!"
	"xx#    $   $ $ #!"
	"xx# $# $ #  $  #!"
	"xx#  $ $  #    #!"
	"### $# #  #### #!"
	"#@#$ $ $  ##   #!"
	"#    $ #$#   # #!"
	"#   $    $ $ $ #!"
	"#####  #########!"
	"xx#      #xxxxxx!"
	"xx#      #xxxxxx!"
	"xx#......#xxxxxx!"
	"xx#......#xxxxxx!"
	"xx#......#xxxxxx!"
	"xx########xxxxxx!",
	"",//d3ruR5D4ulldRur3Duu3rUUruuluuLL3d7DLruLLul3DuurDDrRRl9uuurrdLul9DD9urrdLul8D7uLLDDulDDR"
//	"ur6DRRlDLLrdRRl6urruuLrruu3ld6D4u4ldRRur5DR8urrDrdLLruulld8DLdR9u3uLL4D3ul4DldRRluRR4D5"
//	"ur6DL6ul5ul6DldRRur6DR9uuu4rdrd3rddlldlld3L3rurrurr3ulLu7Lrdd3l3ur6DldRur5D5ur6D5u4rurr"
//	"d5L5ruurr4u9Lrdd3l3ur6DldRur4D4ur5D4u6ruurrdd7L7r4u3lUru7Lrdd3l3ur6DldRur3D3ur4DLLulD",

	// 9
	"xxxxxxxxxx#######!"
	"xxxxxxxxxx#  ...#!"
	"xxxxxx#####  ...#!"
	"xxxxxx#      . .#!"
	"xxxxxx#  ##  ...#!"
	"xxxxxx## ##  ...#!"
	"xxxxx### ########!"
	"xxxxx# $$$ ##xxxx!"
	"x#####  $ $ #####!"
	"##   #$ $   #   #!"
	"#@ $  $    $  $ #!"
	"###### $$ $ #####!"	
	"xxxxx#      #xxxx!"
	"xxxxx########xxxx!",
	"",//rurrd3ruuldrdrruul4ulu7r6l5drddl6ulu6r5l6dllurdr5ulu5r4l5dllurdr4ulu4rdruud4l5dr4dl8ulu4r"
//	"uu3rdrddluulddlu4l9dluudlullulld5rdr7ulu7r6l5dlldrdr6ulu6r5l7d4lulld5rdr7ulu5r4l6d3rulldl"
//	"5ulu4ru3rddluuld4l5drruldl4ulu4rd3rllu4l6d3rdlldl7ulu4rurrld4l7d5rurrd6ldl7ulu4rdrrlu4l9d"
//	"3rulldl8ulu4rurd4l7d5rurrd6ldl7ulu5rldr",

	// 10
	"x###xx#############!"
	"##@####       #   #!"
	"# $$   $$  $ $ ...#!"
	"#  $$$#    $  #...#!"
	"# $   # $$ $$ #...#!"
	"###   #  $    #...#!"
	"#     # $ $ $ #...#!"
	"#    ###### ###...#!"
	"## #  #  $ $  #...#!"
	"#  ## # $$ $ $##..#!"
	"# ..# #  $      #.#!"
	"# ..# # $$$ $$$ #.#!"
	"##### #       # #.#!"
	"xxxx# ######### #.#!"
	"xxxx#           #.#!"
	"xxxx###############!",
	"",//dr4drdl5ulldrur4dl3dldr4urrdlrdr6d10r4u3luurdldlluur3uluurru3rur6d5u3lu3ld6rur12d5ul6u6l3dr"
//	"uulu5rur5d4u5l3druulu5rur11d5ul5u4lddrulu3rur4d3u5l6dlddr7ulu6rur10d5ul4u5l10d4l4u3rdr6ulu5r"
//	"ur3duu5l6d4lddrurrdr7ulu6rur9d5ul3u5l10dludllurrdr9ulu5rurddu5l7d4lu3rdr6ulu6rur8d5uluu5l9dll"
//	"urdr8ulu5rurd5l6d3ldrrdr7ulu6rur7d5ulu5l6d4ldd3rdr8ulu5rur7d6u3ld6lddrrdr3ulu6rur6d5u3lu6ld9r"
//	"ur6d5u13ldllu13rur5d4u12lddrulu11rur4d3u11l3dlulddl4dldr5urrdlrdr6d10r4u3lul6ulu5rur3duu5l6d"
//	"l3dr5ulur4drruldl6ulu6rur5d4u4l4dl3ulu5rur4d3u4l3dll3dlddr7ulu6rur3duu4l3d6ld3rullu3r4dlddr7u"
//	"lu5rurddu3l3dluulu4rurd5l10drudruuldl8ulu6rurddu6l8d4rd3ldl9ulu6rurd6l6drrddrdlldl9ud4lu3ru6r"
//	"3lddllulu5r10ld4ldrur3dl4d4urrdlul3duur5u11r",

};

// number of levels
const int LevNum = count_of(Levels)/2;
