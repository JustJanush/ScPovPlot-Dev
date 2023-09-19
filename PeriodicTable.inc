/****h* ScPovPlot3D/PeriodicTable.inc
* PURPOSE
*   This helper module contains definition of periodic table. The table is designed
*   for  use with Potential.inc and VectorField.inc modules by now. It can be overriden 
*   if required.
*    |html <hr width=50% align="left">
* VERSION
*  4.0.0.#, tested on PovRay 3.7.
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo{at}agh.edu.pl, janusz.opila{at}gmail.com
*   Dept. of Applied Informatics,
*   href:https://www.facebook.com/KatedraInformatykiStosowanejWZAGH/
*   href:http://kis.zarz.agh.edu.pl/
*   AGH University of Science & Technology, Cracow, Poland href:http://www.zarz.agh.edu.pl/English/index.asp
*   Maintained by Janusz Opi³a‚a Ph.D.
*   Homepage: href:http://scpovplot3d.sourceforge.net
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a‚a Ph.D.
*  AGH University of Science and Technology
* HISTORY
*  2023-08-22 - minor correction in PeriodicTabale base (element [0][0] changed from '9' to '22')
*
****
  end of RoboDoc comment
****/

#ifndef(PeriodicTable_INC)
#declare PeriodicTable_INC = "2023-08-22";
#version 3.7;

#local _FileName   = "PeriodicTable.INC";
#local _FileVerMaj = "4.0.1";
#local _FileVerMin = PeriodicTable_INC;
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

#include "colors.inc"
#include "textures.inc"
#include "Cameras.inc"
#include "ColorMaps.inc"

// dictionary of elements  - external interface 
#declare _NElements = 22; // 1.._NElements
#declare PeriodicTable = array[_NElements+1][6]{ 
   // R,  R_VdWalls, LJ eps,  Hue,  Sat,   Lumia -- all these values may be overridden on demand of course
   //                                               TODO: if Hue<0 it refers to the "materials" table
    {  22,  1.00,     1.000,    0,  0.0,   0.0} // number of elements in the table ;)
   ,{0.31,  1.20,     1.000,    0,  0.0,   0.8} //  1  H - hydrogen 
   ,{0.28,  1.40,     1.000,    0,  1.0,   0.5} //  2 He - helium
   ,{1.28,  1.82,     1.000,    0,  1.0,   0.5} //  3 Li - lithium
   ,{0.96,  1.53,     1.000,    0,  1.0,   0.5} //  4 Be - berilium
   ,{0.84,  1.92,     1.000,    0,  1.0,   0.5} //  5  B - bor 
   ,{0.73,  1.70,     1.000,    0,  0.0,   0.0} //  6  C - Carbon/sp2
   ,{0.71,  1.55,     1.000,  175,  1.0,   0.3} //  7  N - nitrogen 
   ,{0.66,  1.52,     1.000,    0,  1.0,   0.3} //  8  O - Oxygen                                                      
   ,{0.64,  1.35,     1.000,    0,  1.0,   0.3} //  9  F - Fluor
//==================================================================
   ,{0.58,  1.54,     1.000,    0,  1.0,   0.5} // 10 Ne - Neon
   ,{1.66,  2.27,     1.000,    0,  1.0,   0.5} // 11 Na - Sodium
   ,{1.41,  1.73,     1.000,    0,  1.0,   0.5} // 12 Mg - Magnesium
   ,{1.21,  1.84,     1.000,    0,  1.0,   0.5} // 13 Al - Aluminium
   ,{1.11,  2.10,     1.000,    0,  1.0,   0.5} // 14 Si - Silicone
   ,{1.07,  1.80,     1.000,    0,  1.0,   0.5} // 15  P - Phosphorus
   ,{1.05,  1.80,     1.000,   57,  1.0,   0.3} // 16  S - Sulphur
   ,{1.02,  1.75,     1.000,  104,  1.0,   0.3} // 17 Cl - Chlorine
   ,{1.06,  1.88,     1.000,    0,  1.0,   0.5} // 18 Ar - Argon
   ,{2.03,  2.75,     1.000,    0,  1.0,   0.5} // 19  K - Potassium
   ,{1.76,  2.31,     1.000,    0,  1.0,   0.5} // 20 Ca - Calcium
   ,{1.70,  2.11,     1.000,    0,  1.0,   0.5} // 21 Sc - Scandium
   ,{1.47,  1.60,     1.000,    0,  1.0,   0.5} // 22 Ti - Titanium  
};                
                       
#declare _PT = PeriodicTable; // internal representation                       

#end // #ifndef(PeriodicTable_INC)