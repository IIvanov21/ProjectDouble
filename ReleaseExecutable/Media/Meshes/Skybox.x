xof 0303txt 0032

Mesh single_mesh_object {
 24;
 -1000.000000;-1000.000000;-1000.000000;,
 -1000.000000; 1000.000000;-1000.000000;,
  1000.000000;-1000.000000;-1000.000000;,
  1000.000000; 1000.000000;-1000.000000;,
  
  1000.000000;-1000.000000;-1000.000000;,
  1000.000000; 1000.000000;-1000.000000;,
  1000.000000;-1000.000000; 1000.000000;,
  1000.000000; 1000.000000; 1000.000000;,
  
  1000.000000;-1000.000000; 1000.000000;,
  1000.000000; 1000.000000; 1000.000000;,
 -1000.000000;-1000.000000; 1000.000000;,
 -1000.000000; 1000.000000; 1000.000000;,
 
 -1000.000000;-1000.000000; 1000.000000;,
 -1000.000000; 1000.000000; 1000.000000;,
 -1000.000000;-1000.000000;-1000.000000;,
 -1000.000000; 1000.000000;-1000.000000;,
 
 -1000.000000; 1000.000000;-1000.000000;,
 -1000.000000; 1000.000000; 1000.000000;,
  1000.000000; 1000.000000;-1000.000000;,
  1000.000000; 1000.000000; 1000.000000;,
  
 -1000.000000;-1000.000000; 1000.000000;,
 -1000.000000;-1000.000000;-1000.000000;,
  1000.000000;-1000.000000; 1000.000000;;
  1000.000000;-1000.000000;-1000.000000;,

 12;
 3;2,1,0;,
 3;1,2,3;,
 3;6,5,4;,
 3;5,6,7;,
 3;10,9,8;,
 3;9,10,11;,
 3;14,13,12;,
 3;13,14,15;,
 3;18,17,16;,
 3;17,18,19;,
 3;22,21,20;,
 3;21,22,23;;

 MeshNormals {
  24;
   0.000000; 0.000000; 1.000000;,
   0.000000; 0.000000; 1.000000;,
   0.000000; 0.000000; 1.000000;,
   0.000000; 0.000000; 1.000000;,
  -1.000000; 0.000000; 0.000000;,
  -1.000000; 0.000000; 0.000000;,
  -1.000000; 0.000000; 0.000000;,
  -1.000000; 0.000000; 0.000000;,
   0.000000; 0.000000;-1.000000;,
   0.000000; 0.000000;-1.000000;,
   0.000000; 0.000000;-1.000000;,
   0.000000; 0.000000;-1.000000;,
   1.000000; 0.000000; 0.000000;,
   1.000000; 0.000000; 0.000000;,
   1.000000; 0.000000; 0.000000;,
   1.000000; 0.000000; 0.000000;,
   0.000000;-1.000000; 0.000000;,
   0.000000;-1.000000; 0.000000;,
   0.000000;-1.000000; 0.000000;,
   0.000000;-1.000000; 0.000000;,
   0.000000; 1.000000; 0.000000;,
   0.000000; 1.000000; 0.000000;,
   0.000000; 1.000000; 0.000000;,
   0.000000; 1.000000; 0.000000;;
  12;
  3;0,1,2;,
  3;2,1,3;,
  3;4,5,6;,
  3;6,5,7;,
  3;8,9,10;,
  3;10,9,11;,
  3;12,13,14;,
  3;14,13,15;,
  3;16,17,18;,
  3;18,17,19;,
  3;20,21,22;,
  3;22,21,23;;
 }

 MeshTextureCoords {
  24;
  1.000000;0.665000;,
  1.000000;0.334000;,
  0.750000;0.665000;,
  0.750000;0.334000;,
  
  0.750000;0.665000;,
  0.750000;0.334000;,
  0.500000;0.665000;,
  0.500000;0.334000;,
  
  0.500000;0.665000;,
  0.500000;0.334000;,
  0.250000;0.665000;,
  0.250000;0.334000;,
  
  0.250000;0.665000;,
  0.250000;0.334000;,
  0.000000;0.665000;,
  0.000000;0.334000;,
  
  0.251000;0.000000;,
  0.251000;0.333000;,
  0.499000;0.000000;,
  0.499000;0.333000;;
  
  0.251000;0.667000;,
  0.251000;1.000000;,
  0.499000;0.667000;,
  0.499000;1.000000;,
 }

 MeshMaterialList {
  1;
  12;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;
 
  Material {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CubeMapB.jpg";
   }
  }
 }
}