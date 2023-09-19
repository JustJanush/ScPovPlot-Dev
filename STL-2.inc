/****h* ScPovPlot3D/STL-2.inc
* PURPOSE
*   Macros for representation of STL surfaces as mesh of triangles,
*   Enhanced version: all data is stored in a huge array _STLDat[][]
*   for this goal object mesh2{} is employed
*     image:./imgs/STL.png
*     |html <hr width=50% align="left">
*     image:./imgs/bk-UncertaintySTL-B.png
*   Fig.[STL] 3D surface and 3D fuzzy texel modelled by mesh2 object
* VERSION
*  4.0.1, 2021-05-19 - tested on PovRay 3.7.0
* AUTHOR
*   Janusz Opi³a Ph.D.
*   jmo@agh.edu.pl, janusz.opila@gmail.com
*   Dept. of Applied Informatics
*   AGH Univ. of Science & Technology. Cracow, Poland
*   Maintained by Janusz Opi³a Ph.D.
* COPYRIGHT
*   GNU GPL v.3 License
*   (c) 2013-now by Janusz Opi³a Ph.D.
* HISTORY
*  2021-05-20, v4.0.1, comment added
*
****
  end of RoboDoc comment
*/

/*************************************************************/
/**   Written by Janusz Opi³a Ph.D., 2013--                 **/
/**   Maintained by Janusz Opi³a Ph.D.                      **/
/**   jo@zarz.agh.edu.pl, janusz.opila@gmail.com            **/
/**   Dept. of Applied Informatics                          **/
/**   AGH Univ. of Science & Technology. Cracow, Poland     **/
/**   Tested on PovRay 3.7.RC7                              **/
/**   License: GNU GPL                                      **/
/**   Homepage:    http://scpovplot3d.sourceforge.net       **/
/*************************************************************/
/**   file version: 0.1.0 (& luckily still working ;)            **/
/*************************************************************/

#ifndef(STL_INC_2)
#declare STL_INC_2 = "2021-05-20"; // last modification date
// common includes & definitions
#version 3.7;
#declare _FileName   = "STL-2.INC";
#declare _FileVerMaj = "1.1.0";
#declare _FileVerMin = STL_INC_2;

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "glass.inc"
#include "math.inc"
#include "strings.inc"

// here are macros defining some useful cameras
/*********************/
// if in troble - use library switch on command line: "+Lfull_path_to_Plot3Dv3" folder eg. "+LG:\Users\user1\Documents\POV-Ray\Sceny\Plot3Dv3"
#include "Cameras.inc"
#include "CommonDefs.inc"

#declare tmpColour   = rgb <0,0,1>; // well, I like blue colour...
#declare tmpColour1  = rgbf<0.2, 0.3, 0.4, 0.1>;
#declare tmpColour2  = rgbf<1.5, 1.3, 1.2, 0.3>;
#declare tmpInterior = inGlass;
#declare tmpTexture  = txBlue;
#declare tmpMaterial = mtGlass;

/****v* STL-2.inc/DeclareLevel
* PURPOSE
*   DeclareLevel determines what is defined:
*   * 0 = colour only
*   * 2 = also texture
*   * 4 = complete material
*   this value is valid until next "Set...()" declaration is met
* SOURCE */
#declare DeclareLevel = 0; // 2in1 - doc entry & initialization
/*****/
#declare Photons=false;
#declare NodeRadiiCoeff=0.20;
#declare WireRadiiCoeff=NodeRadiiCoeff/2;

#declare SceneMinX=1e15; #declare SceneMaxX=-1e15;
#declare SceneMinY=1e15; #declare SceneMaxY=-1e15;
#declare SceneMinZ=1e15; #declare SceneMaxZ=-1e15;

#declare OuterTexture =
       texture { pigment{ Red_Marble }
                 normal { bumps 0.5 scale 0.05}
                 finish { diffuse 0.85 phong 1}
                 scale 0.25
       } // end of texture

#declare InternTexture =
     texture{ Silver_Metal
                       finish { diffuse 0.9 phong 1 }
                       scale 0.5
     }


#declare StripTexture =
             texture{ Rust
                  normal { agate 1.5 scale 0.25}
                  finish { diffuse 0.9 phong 0.1  }
                  scale 0.5
} // end of texture ---------------------------

#declare ShowPoints=no;

#macro SetShowPoints()
    #declare ShowPoints=yes;
#end


#macro TurnPhotonsDown()
    #declare Photons=false;
#end

#macro TurnPhotonsUp()
    #declare Photons=true;
#end

#macro SetColor( FullColour )
    #declare tmpColour = FullColour ;
    #declare DeclareLevel = 0;
#end

#macro SetRGBColor(R, G, BB)
    #declare tmpColour = rgb<R, G, BB >;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor( RR, GG, BB, FF, TTT )
    #declare tmpColour = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor_1( RR, GG, BB, FF, TTT ) // for hollow bar
    #declare tmpColour1 = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor_2( RR, GG, BB, FF, TTT ) // for hollow bar
    #declare tmpColour2 = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetOuterTexture( Txtr ) // follow it by SetInterior("InteriorDefinition")
    #declare OuterTexture = Txtr;
    #declare DeclareLevel = 2;
#end

#macro SetInternTexture( Txtr ) // follow it by SetInterior("InteriorDefinition")
    #declare InternTexture = Txtr;
    #declare DeclareLevel = 2;
#end

#macro SetStripTexture( Txtr ) // follow it by SetInterior("InteriorDefinition")
    #declare StripTexture = Txtr;
    #declare DeclareLevel = 2;
#end

#macro SetMaterial( FullMaterial )
    #declare tmpMaterial = FullMaterial;
    #declare DeclareLevel = 4;
#end

#macro SetNodeRadiiCoeff(Coeff)
  #declare NodeRadiiCoeff=Coeff;
  #declare WireRadiiCoeff=NodeRadiiCoeff/2;
#end

#macro SetWireRadiiCoeff(Coeff)
  #declare WireRadiiCoeff=Coeff;
#end

#declare M_Glass=    // Glass material
material {
  texture {
    pigment {rgbt 1}
    finish {
      ambient 0.0
      diffuse 0.05
      specular 0.6
      roughness 0.005
      reflection {
        0.1, 1.0
        fresnel on
      }
      conserve_energy
    }
  }
  interior {
    ior 1.5
    fade_power 1001
    fade_distance 0.9
    fade_color <0.5,0.8,0.6>
  }
}


/****f* STL-2.inc/DrawSTLSurface ()
* PURPOSE
*  Draws surface representation in the form of smooth mesh of triangles
* SYNOPSIS
DrawSurface(string filename DataFileName)
* INPUTS
*  DataFileName - valid name of file with data - see example file for details
*  of its structure. Generally speaking first is Xaxis title, Yaxis title,
*  Format of the input file - modified STL file: commas and quotation marks
*  "solid", "ascii"
*  "facet", "normal", -0.205737, 0.80223, 0.560446
*    "outer", loop
*      "vertex", -39.6278, -17.0595, -7.95349
*      "vertex", -39.5592, -16.9846, -8.03552
*      "vertex", -39.7552, -17.0438, -8.02273
*    "endloop"
*  "endfacet"
*  |
*    "facet" ... "endfacet"
*    "facet" ... "endfacet"
*    "facet" ... "endfacet"
*  |
*  "endfacet",
* "endsolid"
* SEE ALSO
*  WireSurf.inc
*  BPatchSurf.inc
*  DrawSurfaceOnBlock ()
****/
#macro DrawSTLSurface(DataFileName)
    #declare Node=<0,0,0>;
    #declare xx = 0.0;
    #declare yy = 0.0;
    #declare zz = 0.0;

// povray coordinates vs. real
    #declare xxp= 0.0;  //  yy
    #declare yyp= 0.0;  //  zz
    #declare zzp= 0.0;  // -xx

    #declare TxtStub1="";
    #declare TxtStub2="";
union{
    #fopen DataFile DataFileName read
    #debug concat("Just reading ", DataFileName,"\n")
    #read(DataFile, TxtStub1, TxtStub2)
#if (Debug>0)  #debug concat( TxtStub1, "   ", TxtStub2, "\n") #end

// obsolete    #local JustRead=0.0;
    #declare FacetCount=0;
    #while(defined(DataFile))

#if (mod(FacetCount,1000)=0)
  #debug concat("FacetCount= ",str(FacetCount,6,0),"\n")
#end
        #read(DataFile, TxtStub1)
#if (Debug>0)  #debug concat(TxtStub1, "\n") #end
       #if (strcmp(TxtStub1, "endsolid"))    // OK, different

            #read(DataFile, TxtStub1, xx, yy, zz) // >> , facet, normal, -0.205737, 0.80223, 0.560446 REAL/discard

            #read(DataFile, TxtStub1, TxtStub2)   // >> ,   outer, loop

            triangle{
                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/1 node
                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                <xxp, yyp, zzp>,

                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/2 node

                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                    <xxp, yyp, zzp>,

                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/3 node
                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                    <xxp, yyp, zzp>

                #read(DataFile, TxtStub1, TxtStub2)             // >> ,   endloop, endfacet

            }

            #if (strcmp(TxtStub2, "endfacet"))
               #debug concat("TxtStub2 = ", TxtStub2, "\n")
            #end
            #declare FacetCount=FacetCount+1;
       #end //    ... #if (strcmp(TxtStub1, "endsolid"))
    #end  // #while()
    #debug concat("All ", str(FacetCount,6,0), " facets successfuly read in ", str(FacetCount,6,0),"\n => now rendering. . . . ")

    #fclose DataFile

/** Legacy **/

     texture { OuterTexture } // end of texture

     interior_texture{ InternTexture }
}
#end    // macro DrawSTLSurface(DataFileName)...


/****f* STL-2.inc/DrawSTLSurfaceArr ()
* PURPOSE
*  Draws surface representation in the form of smooth mesh of triangles, employes huge array _STLDat[][]
* SYNOPSIS
DrawSurface(string filename DataFileName)
* INPUTS
*  DataFileName - valid name of file with data - see example file for details
*  of its structure. Generally speaking first is Xaxis title, Yaxis title,
*  Format of the input file - modified STL file: commas and quotation marks
*  "solid", "ascii"
*  "facet", "normal", -0.205737, 0.80223, 0.560446
*    "outer", loop
*      "vertex", -39.6278, -17.0595, -7.95349
*      "vertex", -39.5592, -16.9846, -8.03552
*      "vertex", -39.7552, -17.0438, -8.02273
*    "endloop"
*  "endfacet"
*  |
*    "facet" ... "endfacet"
*    "facet" ... "endfacet"
*    "facet" ... "endfacet"
*  |
*  "endfacet",
* "endsolid"
* SEE ALSO
*  WireSurf.inc
*  BPatchSurf.inc
*  DrawSurfaceOnBlock ()
****/
#macro DrawSTLSurfaceArr(DataFileName, _N) // _N size of atable
    #declare _STLDat = array[_N+1][4];     // total/normal+three vertices as <,,>. First node at i=1
    #declare _STLDat[0][0] = <1,0,0>;
    #declare  Node = <0,0,0>;
    #declare  xx = 0.0;
    #declare  yy = 0.0;
    #declare  zz = 0.0;

// povray coordinates vs. real
    #declare xxp = 0.0;  //  yy
    #declare yyp = 0.0;  //  zz
    #declare zzp = 0.0;  // -xx

// "mass" center
    #declare gx  = 0.0;  //  yy
    #declare gy  = 0.0;  //  zz
    #declare gz  = 0.0;  // -xx

    #declare TxtStub1 = "";
    #declare TxtStub2 = "";

    #fopen DataFile DataFileName read
    #debug concat("Just reading ", DataFileName,"\n")
    #read(DataFile, TxtStub1, TxtStub2)
#if (Debug>0)  #debug concat( TxtStub1, "   ", TxtStub2, "\n") #end

    #for(FacetCount, 1, _N)
        #if (mod(FacetCount, 1000)=0)
          #debug concat("FacetCount= ",str(FacetCount,6,0),"\n")
        #end
        #read(DataFile, TxtStub1)
#if (Debug>0)  #debug concat(TxtStub1, "\n") #end
            #read(DataFile, TxtStub1, xx, yy, zz) // >> , facet, normal, -0.205737, 0.80223, 0.560446 REAL/discard

            #read(DataFile, TxtStub1, TxtStub2)   // >> ,   outer, loop
            #declare _STLDat[FacetCount][0] = <xx, yy, zz>;

            // 1st vertex
            #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/1 node
                #declare xxp =  yy; #declare yyp =  zz; #declare zzp = -xx; // conversion to internal coord system
                #declare gx=gx+xxp; #declare gy=gy+yyp; #declare gz=gz+zzp;
                #if (FacetCount>1)
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                #else
                    #declare SceneMinX=xxp; #declare SceneMinY=yyp; #declare SceneMinZ=zzp;
                    #declare SceneMaxX=xxp; #declare SceneMaxY=yyp; #declare SceneMaxZ=zzp;
                #end
            #declare _STLDat[FacetCount][1] = <xxp, yyp, zzp>;

            // 2nd vertex
            #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/2 node
                #declare xxp =  yy; #declare yyp =  zz; #declare zzp = -xx;  // conversion to internal coord system
                #declare gx=gx+xxp; #declare gy=gy+yyp; #declare gz=gz+zzp;
                #if (FacetCount>1)
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                #else
                    #declare SceneMinX=xxp; #declare SceneMinY=yyp; #declare SceneMinZ=zzp;
                    #declare SceneMaxX=xxp; #declare SceneMaxY=yyp; #declare SceneMaxZ=zzp;
                #end

            #declare _STLDat[FacetCount][2] = <xxp, yyp, zzp>;

            // 3rd vertex
            #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/3 node
                #declare xxp =  yy; #declare yyp =  zz; #declare zzp = -xx;   // conversion to internal coord system
                #declare gx=gx+xxp; #declare gy=gy+yyp; #declare gz=gz+zzp;
                #if (FacetCount>1)
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                #else
                    #declare SceneMinX=xxp; #declare SceneMinY=yyp; #declare SceneMinZ=zzp;
                    #declare SceneMaxX=xxp; #declare SceneMaxY=yyp; #declare SceneMaxZ=zzp;
                #end

            #declare _STLDat[FacetCount][3] = <xxp, yyp, zzp>;
            #read(DataFile, TxtStub1, TxtStub2)             // >> ,   endloop, endfacet
    #end
    #fclose DataFile
#debug concat("All ", str(_N,6,0), " facets successfuly read in ", "\n => now rendering. . . .\n ")

// Now everything is in memory
     #declare _STLDat[0][0] = <gx, gy, gz>/_N/3; // "mass" center
     #declare _STLDat[0][1] = <SceneMinX, SceneMinY, SceneMinZ>; // min. point
     #declare _STLDat[0][2] = <SceneMaxX, SceneMaxY, SceneMaxZ>; // max. point

#debug concat("masscenter: ", Vstr(_STLDat[0][0],10,5), "\n")
#debug concat("min extent: ", Vstr(_STLDat[0][1],10,5), "\n")
#debug concat("max extent: ", Vstr(_STLDat[0][2],10,5), "\n")

    union{
         #for(FacetCount, 1, _N)
                triangle{
                    _STLDat[FacetCount][1],
                    _STLDat[FacetCount][2],
                    _STLDat[FacetCount][3]
                }
         #end

         texture { OuterTexture } // end of texture
         interior_texture{ InternTexture }
    }
#end    // macro DrawSTLSurface(DataFileName)...
///===============================================///



/****f* STL-2.inc/ScanSTLSurface()
* PURPOSE
*  Draws surface representation in the form of smooth mesh of triangles
* SYNOPSIS
DrawSurface(string filename DataFileName)
* INPUTS
*  DataFileName - valid name of file with data - see example file for details
*  of its structure. Generally speaking first is Xaxis title, Yaxis title,
*  Format of the input file - modified STL file: added commas and quotation marks
*  "solid", "ascii"
*  "facet", "normal", -0.205737, 0.80223, 0.560446
*    "outer", loop
*      "vertex", -39.6278, -17.0595, -7.95349
*      "vertex", -39.5592, -16.9846, -8.03552
*      "vertex", -39.7552, -17.0438, -8.02273
*    "endloop"
*  "endfacet"
*  |
*    "facet" ... "endfacet"
*    "facet" ... "endfacet"
*    "facet" ... "endfacet"
*  |
*  "endfacet",
* "endsolid"
*  Start  - Integer - begin scanning strip at this position
*  Window - Integer - width of scanning window (for example 1000)
* SEE ALSO
*  WireSurf.inc
*  BPatchSurf.inc
*  DrawSurfaceOnBlock ()
*  DrawSTLSurface ()
****/
#macro ScanSTLSurface(DataFileName, Start, Window)
    #declare Node=<0,0,0>;
    #declare xx = 0.0;
    #declare yy = 0.0;
    #declare zz = 0.0;

// povray coordinates vs. real
    #declare xxp= 0.0;  //  yy
    #declare yyp= 0.0;  //  zz
    #declare zzp= 0.0;  // -xx

    #declare TxtStub1="";
    #declare TxtStub2="";

    #fopen DataFile DataFileName read
    #debug concat("Just reading ", DataFileName,"\n")
    #read(DataFile, TxtStub1, TxtStub2)
#if (Debug>0)  #debug concat( TxtStub1, "   ", TxtStub2, "\n") #end

// obsolete    #local JustRead=0.0;
    #declare FacetCount=0;
union{
union{ // 1 - introductory part
    #while(defined(DataFile) & (FacetCount<Start))

#if (mod(FacetCount,Window)=0)   #debug concat("FacetCount= ",str(FacetCount,6,0),"\n") #end
       #read(DataFile, TxtStub1)
#if (Debug>0)  #debug concat(TxtStub1, "\n") #end
       #if (strcmp(TxtStub1, "endsolid"))    // OK, different

            #read(DataFile, TxtStub1, xx, yy, zz) // >> , facet, normal, -0.205737, 0.80223, 0.560446 REAL/discard

            #read(DataFile, TxtStub1, TxtStub2)   // >> ,   outer, loop

            triangle{
                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/1 node
                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                <xxp, yyp, zzp>,

                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/2 node

                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                    <xxp, yyp, zzp>,

                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/3 node
                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                    <xxp, yyp, zzp>

                #read(DataFile, TxtStub1, TxtStub2)             // >> ,   endloop, endfacet

            }

            #if (strcmp(TxtStub2, "endfacet"))  #debug concat("TxtStub2 = ", TxtStub2, "\n") #end
            #declare FacetCount=FacetCount+1;
       #end //    ... #if (strcmp(TxtStub1, "endsolid"))
    #end  // while()
/** Legacy **/

    texture { OuterTexture } // end of texture

    interior_texture{ InternTexture }
}


union{ // scanning strip
    #debug concat("Now reading strip window", DataFileName,"\n")
    #declare EndStrip=Start+Window;
    #while(defined(DataFile) & (FacetCount<EndStrip))

#if (mod(FacetCount,Window)=0)
  #debug concat("FacetCount= ",str(FacetCount,6,0),"\n")
#end
        #read(DataFile, TxtStub1)
#if (Debug>0)  #debug concat(TxtStub1, "\n") #end
       #if (strcmp(TxtStub1, "endsolid"))    // OK, different

            #read(DataFile, TxtStub1, xx, yy, zz) // >> , facet, normal, -0.205737, 0.80223, 0.560446 REAL/discard

            #read(DataFile, TxtStub1, TxtStub2)   // >> ,   outer, loop

            triangle{
                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/1 node
                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                <xxp, yyp, zzp>,

                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/2 node

                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                    <xxp, yyp, zzp>,

                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/3 node
                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                    <xxp, yyp, zzp>

                #read(DataFile, TxtStub1, TxtStub2)             // >> ,   endloop, endfacet

            }

            #if (strcmp(TxtStub2, "endfacet"))
               #debug concat("\"endfacet\" expected, instead = ", TxtStub2, "found. \n")
            #end
            #declare FacetCount=FacetCount+1;
       #end //    ... #if (strcmp(TxtStub1, "endsolid"))
    #end

/** Legacy **/

     texture { pigment {color Red} } // end of texture

     interior_texture{ InternTexture }
}


/*************************************/

union{ // 3 - External part
    #debug concat("Just reading ", DataFileName,"\n")

    #while(defined(DataFile))

#if (mod(FacetCount,Window)=0)
  #debug concat("FacetCount= ",str(FacetCount,6,0),"\n")
#end
        #read(DataFile, TxtStub1)
#if (Debug>0)  #debug concat(TxtStub1, "\n") #end
       #if (strcmp(TxtStub1, "endsolid"))    // OK, different

            #read(DataFile, TxtStub1, xx, yy, zz) // >> , facet, normal, -0.205737, 0.80223, 0.560446 REAL/discard

            #read(DataFile, TxtStub1, TxtStub2)   // >> ,   outer, loop

            triangle{
                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/1 node
                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                <xxp, yyp, zzp>,

                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/2 node

                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                    <xxp, yyp, zzp>,

                #read(DataFile, TxtStub1, xx, yy, zz) // >> ,     "vertex", -39.6278, -17.0595, -7.95349 REAL/3 node
                    #declare xxp =  yy;   #declare yyp =  zz;   #declare zzp = -xx;
                    #if (xxp<SceneMinX) #declare SceneMinX=xxp; #end
                    #if (xxp>SceneMaxX) #declare SceneMaxX=xxp; #end

                    #if (yyp<SceneMinY) #declare SceneMinY=yyp; #end
                    #if (yyp>SceneMaxY) #declare SceneMaxY=yyp; #end

                    #if (zzp<SceneMinZ) #declare SceneMinZ=zzp; #end
                    #if (zzp>SceneMaxZ) #declare SceneMaxZ=zzp; #end
                    <xxp, yyp, zzp>

                #read(DataFile, TxtStub1, TxtStub2)             // >> ,   endloop, endfacet

            }

            #if (strcmp(TxtStub2, "endfacet"))
               #debug concat("TxtStub2 = ", TxtStub2, "\n")
            #end
            #declare FacetCount=FacetCount+1;
       #end //    ... #if (strcmp(TxtStub1, "endsolid"))
    #end



/** Legacy **/

     texture { InternTexture } // end of texture

     interior_texture{ InternTexture }
}

} // end general union
    #debug concat("All ", str(FacetCount,6,0), " facets successfuly read in ", str(FacetCount,6,0),"\n => now rendering. . . . ")

    #fclose DataFile


#end    // macro ScanSTLSurface(DataFileName)...




#end    // ifndef STL_INC_2...
