/****h* ScPovPlot3D/scFinish.inc
* PURPOSE
*   Several useful finishes. This library silently includes system library file "finish.inc" and defined finishes there. 
*     image:./imgs/bk-SuperEllipsoidPhotonsSm.png
*     
*   Fig.[scFinishes] Scene employing finishes declared in this library
*   |html <hr width=50% align="left">
* VERSION
*  4.0.0, tested on PovRay 3.7.
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo{at}agh.edu.pl, janusz.opila{at}gmail.com
*   Dept. of Applied Informatics, 
*   href:https://www.facebook.com/KatedraInformatykiStosowanejWZAGH/
*   href:http://kis.zarz.agh.edu.pl/
*   AGH University of Science & Technology, Cracow, Poland href:http://www.zarz.agh.edu.pl/English/index.asp
*   Maintained by Janusz Opi³a Ph.D.
*   Homepage: http://scpovplot3d.sourceforge.net   
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a Ph.D.         
*  AGH University of Science and Technology
* HISTORY
*  2016-02-03 - 'Graphite' RGB color added
****
  end of RoboDoc comment
****/

#version 3.7;
#ifndef(scFinish_INC)
#declare scFinish_INC = "2016-02-03";        

#include "colors.inc"
#include "finish.inc"
#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

#declare _FileName   = "scFinish.inc";
#declare _FileVerMaj = "4.0.0";
#declare _FileVerMin = scFinish_INC;
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#declare scDullMirror  = finish{ phong 1 phong_size 300 diffuse 0.51 reflection 0.23}
#declare scPianoBlack  = texture{
    pigment{color Mica} 
    pigment{color rgb<.02,0,.02>} 
    finish{ Dull }
};  

#declare scPianoBlack2  = texture{
    pigment{color Mica} 
    pigment{color rgbt<.02,0,.02,.05>} 
    finish{ scDullMirror subsurface {translucency <.402,0.,.02>}
    }
};  

#declare Graphite = rgb 0.10; // RGB color
#declare txMica = scPianoBlack;

#end

