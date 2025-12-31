Output Files
============

``stomms_meshgen`` outputs Simmetrix model file (``.smd``), Simmetrix mesh file (``.sms``), and adios2 file (``.bp``) containing all the mesh information along with the phsyical and physics classification information.

Sample Adios2 Mesh File
-----------------------
A sample output ADIOS2 file from TOMMS is provided here. STOMMS supports similar output file but with multiple planes. Once STOMMS development is finished, sample output file will be updated here. This file contains a version number of the mesh generator, variables to define an Omega_h mesh, model adjacency information, and physics classification information.  
The hierarchy in this file is: 
 
* Mesh Generator Version (TOMMS, STOMMS version)
* MeshName
   * Planes
      * plane 0
            * Variables for Omega_h mesh definition
            * Model adjacencies
                * Dimension of the model entity
                    * Dimension of adjacent entities
            * Physics Classification
                * Dimension of the model entity
                    * Physics property associated with the model entity  
      * plane N-1
            * Variables for Omega_h mesh definition
            * Model adjacencies
                * Dimension of the model entity
                    * Dimension of adjacent entities
            * Physics Classification
                * Dimension of the model entity
                    * Physics property associated with the model entity  

.. code-block:: text

  int32_t  TommsVersion                                                                  scalar
  int32_t  tommsMesh/planes/0/comm_size                                                  {1}
  int32_t  tommsMesh/planes/0/dim                                                        {1}
  int32_t  tommsMesh/planes/0/downward_adj/1_to_0                                        {644552, 2}
  int32_t  tommsMesh/planes/0/downward_adj/2_to_1                                        {429226, 3}
  int8_t   tommsMesh/planes/0/downward_codes/2                                           {429226, 3}
  int32_t  tommsMesh/planes/0/family                                                     {1}
  int32_t  tommsMesh/planes/0/mesh_version                                               {1}
  int32_t  tommsMesh/planes/0/modelAdj/0/toEdge/data                                     {699, 1}
  int32_t  tommsMesh/planes/0/modelAdj/0/toEdge/range                                    {318, 1}
  int32_t  tommsMesh/planes/0/modelAdj/0/toFace/data                                     {610, 1}
  int32_t  tommsMesh/planes/0/modelAdj/0/toFace/range                                    {318, 1}
  int32_t  tommsMesh/planes/0/modelAdj/0/vertexIds                                       {317, 1}
  int32_t  tommsMesh/planes/0/modelAdj/1/edgeIds                                         {406, 1}
  int32_t  tommsMesh/planes/0/modelAdj/1/toFace/data                                     {610, 1}
  int32_t  tommsMesh/planes/0/modelAdj/1/toFace/range                                    {407, 1}
  int32_t  tommsMesh/planes/0/modelAdj/1/toVertex/data                                   {699, 1}
  int32_t  tommsMesh/planes/0/modelAdj/1/toVertex/range                                  {407, 1}
  int32_t  tommsMesh/planes/0/modelAdj/2/faceIds                                         {203, 1}
  int32_t  tommsMesh/planes/0/modelAdj/2/toEdge/data                                     {610, 1}
  int32_t  tommsMesh/planes/0/modelAdj/2/toEdge/range                                    {204, 1}
  int32_t  tommsMesh/planes/0/modelAdj/2/toVertex/data                                   {610, 1}
  int32_t  tommsMesh/planes/0/modelAdj/2/toVertex/range                                  {204, 1}
  int32_t  tommsMesh/planes/0/nghost_layers                                              {1}
  int32_t  tommsMesh/planes/0/nverts                                                     {1}
  int32_t  tommsMesh/planes/0/parting                                                    {1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/closed/fluxIds                  {112, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/closed/modelEdges/data          {112, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/closed/modelEdges/range         {113, 1}
  double   tommsMesh/planes/0/physicsClassification/edge/closed/psi                      {112, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/open/fluxIds                    {85, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/open/modelEdges/data            {85, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/open/modelEdges/range           {86, 1}
  double   tommsMesh/planes/0/physicsClassification/edge/open/psi                        {85, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/separatrix/fluxIds              {2, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/separatrix/modelEdges/data      {7, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/separatrix/modelEdges/range     {3, 1}
  double   tommsMesh/planes/0/physicsClassification/edge/separatrix/psi                  {2, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/wall/fluxIds                    {1, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/wall/modelEdges/data            {202, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/edge/wall/modelEdges/range           {2, 1}
  double   tommsMesh/planes/0/physicsClassification/edge/wall/psi                        {1, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/face/core                            {113, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/face/nearVacuumRegion/highFieldSide  {2, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/face/nearVacuumRegion/lowFieldSide   {2, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/face/plasmaEdgeRegion/highFieldSide  {10, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/face/plasmaEdgeRegion/lowFieldSide   {13, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/face/privateRegion/lower             {34, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/face/privateRegion/upper             {15, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/face/sol                             {14, 1}
  int32_t  tommsMesh/planes/0/physicsClassification/vertex/criticalPoints/oPoint/1       scalar
  int32_t  tommsMesh/planes/0/physicsClassification/vertex/criticalPoints/xPoint/1       scalar
  int32_t  tommsMesh/planes/0/physicsClassification/vertex/criticalPoints/xPoint/2       scalar
  int32_t  tommsMesh/planes/0/rank                                                       {1}
  int8_t   tommsMesh/planes/0/tags/0/class_dim/data                                      {215327, 1}
  int32_t  tommsMesh/planes/0/tags/0/class_id/data                                       {215327, 1}
  double   tommsMesh/planes/0/tags/0/coordinates/data                                    {215327, 2}
  int64_t  tommsMesh/planes/0/tags/0/global/data                                         {215327, 1}
  int8_t   tommsMesh/planes/0/tags/1/class_dim/data                                      {644552, 1}
  int32_t  tommsMesh/planes/0/tags/1/class_id/data                                       {644552, 1}
  int64_t  tommsMesh/planes/0/tags/1/global/data                                         {644552, 1}
  int8_t   tommsMesh/planes/0/tags/2/class_dim/data                                      {429226, 1}
  int32_t  tommsMesh/planes/0/tags/2/class_id/data                                       {429226, 1}
  int64_t  tommsMesh/planes/0/tags/2/global/data                                         {429226, 1}
