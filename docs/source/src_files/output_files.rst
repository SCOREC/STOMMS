Output Files
============

``stomms_meshgen`` outputs Simmetrix model file (``.smd``), Simmetrix mesh file (``.sms``), and adios2 file (``.bp``) containing all the mesh information along with the phsyical and physics classification information. Moreover, it contains the background grid information for tokamak cases.

Sample Adios2 Mesh File
-----------------------
A sample output ADIOS2 file from TOMMS is provided here. STOMMS supports similar output file but with multiple planes. In the example below, an adios file contains the data for a model/mesh based on 7 poloidal planes. This file contains a version number of the mesh generator, variables to define an Omega_h mesh, model adjacency information, and physics classification information.  
The hierarchy in this file is: 
 
* Mesh Generator Version (TOMMS, STOMMS version)
* MeshName
   * Background grid information
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


Sample Adios2 output file for multi plane mesh
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

  string   StommsVersion                                                             scalar
  int32_t  stommsMesh/planes/0/comm_size                                             {1}
  int32_t  stommsMesh/planes/0/dim                                                   {1}
  int32_t  stommsMesh/planes/0/downward_adj/1_to_0                                   {10738, 2}
  int32_t  stommsMesh/planes/0/downward_adj/2_to_1                                   {7063, 3}
  int8_t   stommsMesh/planes/0/downward_codes/2                                      {7063, 3}
  int32_t  stommsMesh/planes/0/family                                                {1}
  int32_t  stommsMesh/planes/0/mesh_version                                          {1}
  int32_t  stommsMesh/planes/0/modelAdj/0/toEdge/range                               {2, 1}
  int32_t  stommsMesh/planes/0/modelAdj/0/toFace/data                                {1, 1}
  int32_t  stommsMesh/planes/0/modelAdj/0/toFace/range                               {2, 1}
  int32_t  stommsMesh/planes/0/modelAdj/0/vertexIds                                  {1, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/edgeIds                                    {13, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/toFace/data                                {25, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/toFace/range                               {14, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/faceIds                                    {13, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/toEdge/data                                {25, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/toEdge/range                               {14, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/toVertex/data                              {1, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/0/nghost_layers                                         {1}
  int32_t  stommsMesh/planes/0/nverts                                                {1}
  int32_t  stommsMesh/planes/0/parting                                               {1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/closed/fluxIds             {13, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/closed/modelEdges/data     {13, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/closed/modelEdges/range    {14, 1}
  double   stommsMesh/planes/0/physicsClassification/edge/closed/psi                 {13, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/face/core                       {13, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/vertex/criticalPoints/oPoint/1  scalar
  int32_t  stommsMesh/planes/0/rank                                                  {1}
  int8_t   stommsMesh/planes/0/tags/0/class_dim/data                                 {3676, 1}
  int32_t  stommsMesh/planes/0/tags/0/class_id/data                                  {3676, 1}
  double   stommsMesh/planes/0/tags/0/coordinates/data                               {3676, 2}
  int64_t  stommsMesh/planes/0/tags/0/global/data                                    {3676, 1}
  int8_t   stommsMesh/planes/0/tags/1/class_dim/data                                 {10738, 1}
  int32_t  stommsMesh/planes/0/tags/1/class_id/data                                  {10738, 1}
  int64_t  stommsMesh/planes/0/tags/1/global/data                                    {10738, 1}
  int8_t   stommsMesh/planes/0/tags/2/class_dim/data                                 {7063, 1}
  int32_t  stommsMesh/planes/0/tags/2/class_id/data                                  {7063, 1}
  int64_t  stommsMesh/planes/0/tags/2/global/data                                    {7063, 1}
  int32_t  stommsMesh/planes/1/comm_size                                             {1}
  int32_t  stommsMesh/planes/1/dim                                                   {1}
  int32_t  stommsMesh/planes/1/downward_adj/1_to_0                                   {10738, 2}
  int32_t  stommsMesh/planes/1/downward_adj/2_to_1                                   {7063, 3}
  int8_t   stommsMesh/planes/1/downward_codes/2                                      {7063, 3}
  int32_t  stommsMesh/planes/1/family                                                {1}
  int32_t  stommsMesh/planes/1/mesh_version                                          {1}
  int32_t  stommsMesh/planes/1/modelAdj/0/toEdge/range                               {2, 1}
  int32_t  stommsMesh/planes/1/modelAdj/0/toFace/data                                {1, 1}
  int32_t  stommsMesh/planes/1/modelAdj/0/toFace/range                               {2, 1}
  int32_t  stommsMesh/planes/1/modelAdj/0/vertexIds                                  {1, 1}
  int32_t  stommsMesh/planes/1/modelAdj/1/edgeIds                                    {13, 1}
  int32_t  stommsMesh/planes/1/modelAdj/1/toFace/data                                {25, 1}
  int32_t  stommsMesh/planes/1/modelAdj/1/toFace/range                               {14, 1}
  int32_t  stommsMesh/planes/1/modelAdj/1/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/1/modelAdj/2/faceIds                                    {13, 1}
  int32_t  stommsMesh/planes/1/modelAdj/2/toEdge/data                                {25, 1}
  int32_t  stommsMesh/planes/1/modelAdj/2/toEdge/range                               {14, 1}
  int32_t  stommsMesh/planes/1/modelAdj/2/toVertex/data                              {1, 1}
  int32_t  stommsMesh/planes/1/modelAdj/2/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/1/nghost_layers                                         {1}
  int32_t  stommsMesh/planes/1/nverts                                                {1}
  int32_t  stommsMesh/planes/1/parting                                               {1}
  int32_t  stommsMesh/planes/1/physicsClassification/edge/closed/fluxIds             {13, 1}
  int32_t  stommsMesh/planes/1/physicsClassification/edge/closed/modelEdges/data     {13, 1}
  int32_t  stommsMesh/planes/1/physicsClassification/edge/closed/modelEdges/range    {14, 1}
  double   stommsMesh/planes/1/physicsClassification/edge/closed/psi                 {13, 1}
  int32_t  stommsMesh/planes/1/physicsClassification/face/core                       {13, 1}
  int32_t  stommsMesh/planes/1/physicsClassification/vertex/criticalPoints/oPoint/1  scalar
  int32_t  stommsMesh/planes/1/rank                                                  {1}
  int8_t   stommsMesh/planes/1/tags/0/class_dim/data                                 {3676, 1}
  int32_t  stommsMesh/planes/1/tags/0/class_id/data                                  {3676, 1}
  double   stommsMesh/planes/1/tags/0/coordinates/data                               {3676, 2}
  int64_t  stommsMesh/planes/1/tags/0/global/data                                    {3676, 1}
  int8_t   stommsMesh/planes/1/tags/1/class_dim/data                                 {10738, 1}
  int32_t  stommsMesh/planes/1/tags/1/class_id/data                                  {10738, 1}
  int64_t  stommsMesh/planes/1/tags/1/global/data                                    {10738, 1}
  int8_t   stommsMesh/planes/1/tags/2/class_dim/data                                 {7063, 1}
  int32_t  stommsMesh/planes/1/tags/2/class_id/data                                  {7063, 1}
  int64_t  stommsMesh/planes/1/tags/2/global/data                                    {7063, 1}
  int32_t  stommsMesh/planes/2/comm_size                                             {1}
  int32_t  stommsMesh/planes/2/dim                                                   {1}
  int32_t  stommsMesh/planes/2/downward_adj/1_to_0                                   {10738, 2}
  int32_t  stommsMesh/planes/2/downward_adj/2_to_1                                   {7063, 3}
  int8_t   stommsMesh/planes/2/downward_codes/2                                      {7063, 3}
  int32_t  stommsMesh/planes/2/family                                                {1}
  int32_t  stommsMesh/planes/2/mesh_version                                          {1}
  int32_t  stommsMesh/planes/2/modelAdj/0/toEdge/range                               {2, 1}
  int32_t  stommsMesh/planes/2/modelAdj/0/toFace/data                                {1, 1}
  int32_t  stommsMesh/planes/2/modelAdj/0/toFace/range                               {2, 1}
  int32_t  stommsMesh/planes/2/modelAdj/0/vertexIds                                  {1, 1}
  int32_t  stommsMesh/planes/2/modelAdj/1/edgeIds                                    {13, 1}
  int32_t  stommsMesh/planes/2/modelAdj/1/toFace/data                                {25, 1}
  int32_t  stommsMesh/planes/2/modelAdj/1/toFace/range                               {14, 1}
  int32_t  stommsMesh/planes/2/modelAdj/1/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/2/modelAdj/2/faceIds                                    {13, 1}
  int32_t  stommsMesh/planes/2/modelAdj/2/toEdge/data                                {25, 1}
  int32_t  stommsMesh/planes/2/modelAdj/2/toEdge/range                               {14, 1}
  int32_t  stommsMesh/planes/2/modelAdj/2/toVertex/data                              {1, 1}
  int32_t  stommsMesh/planes/2/modelAdj/2/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/2/nghost_layers                                         {1}
  int32_t  stommsMesh/planes/2/nverts                                                {1}
  int32_t  stommsMesh/planes/2/parting                                               {1}
  int32_t  stommsMesh/planes/2/physicsClassification/edge/closed/fluxIds             {13, 1}
  int32_t  stommsMesh/planes/2/physicsClassification/edge/closed/modelEdges/data     {13, 1}
  int32_t  stommsMesh/planes/2/physicsClassification/edge/closed/modelEdges/range    {14, 1}
  double   stommsMesh/planes/2/physicsClassification/edge/closed/psi                 {13, 1}
  int32_t  stommsMesh/planes/2/physicsClassification/face/core                       {13, 1}
  int32_t  stommsMesh/planes/2/physicsClassification/vertex/criticalPoints/oPoint/1  scalar
  int32_t  stommsMesh/planes/2/rank                                                  {1}
  int8_t   stommsMesh/planes/2/tags/0/class_dim/data                                 {3676, 1}
  int32_t  stommsMesh/planes/2/tags/0/class_id/data                                  {3676, 1}
  double   stommsMesh/planes/2/tags/0/coordinates/data                               {3676, 2}
  int64_t  stommsMesh/planes/2/tags/0/global/data                                    {3676, 1}
  int8_t   stommsMesh/planes/2/tags/1/class_dim/data                                 {10738, 1}
  int32_t  stommsMesh/planes/2/tags/1/class_id/data                                  {10738, 1}
  int64_t  stommsMesh/planes/2/tags/1/global/data                                    {10738, 1}
  int8_t   stommsMesh/planes/2/tags/2/class_dim/data                                 {7063, 1}
  int32_t  stommsMesh/planes/2/tags/2/class_id/data                                  {7063, 1}
  int64_t  stommsMesh/planes/2/tags/2/global/data                                    {7063, 1}
  int32_t  stommsMesh/planes/3/comm_size                                             {1}
  int32_t  stommsMesh/planes/3/dim                                                   {1}
  int32_t  stommsMesh/planes/3/downward_adj/1_to_0                                   {10738, 2}
  int32_t  stommsMesh/planes/3/downward_adj/2_to_1                                   {7063, 3}
  int8_t   stommsMesh/planes/3/downward_codes/2                                      {7063, 3}
  int32_t  stommsMesh/planes/3/family                                                {1}
  int32_t  stommsMesh/planes/3/mesh_version                                          {1}
  int32_t  stommsMesh/planes/3/modelAdj/0/toEdge/range                               {2, 1}
  int32_t  stommsMesh/planes/3/modelAdj/0/toFace/data                                {1, 1}
  int32_t  stommsMesh/planes/3/modelAdj/0/toFace/range                               {2, 1}
  int32_t  stommsMesh/planes/3/modelAdj/0/vertexIds                                  {1, 1}
  int32_t  stommsMesh/planes/3/modelAdj/1/edgeIds                                    {13, 1}
  int32_t  stommsMesh/planes/3/modelAdj/1/toFace/data                                {25, 1}
  int32_t  stommsMesh/planes/3/modelAdj/1/toFace/range                               {14, 1}
  int32_t  stommsMesh/planes/3/modelAdj/1/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/3/modelAdj/2/faceIds                                    {13, 1}
  int32_t  stommsMesh/planes/3/modelAdj/2/toEdge/data                                {25, 1}
  int32_t  stommsMesh/planes/3/modelAdj/2/toEdge/range                               {14, 1}
  int32_t  stommsMesh/planes/3/modelAdj/2/toVertex/data                              {1, 1}
  int32_t  stommsMesh/planes/3/modelAdj/2/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/3/nghost_layers                                         {1}
  int32_t  stommsMesh/planes/3/nverts                                                {1}
  int32_t  stommsMesh/planes/3/parting                                               {1}
  int32_t  stommsMesh/planes/3/physicsClassification/edge/closed/fluxIds             {13, 1}
  int32_t  stommsMesh/planes/3/physicsClassification/edge/closed/modelEdges/data     {13, 1}
  int32_t  stommsMesh/planes/3/physicsClassification/edge/closed/modelEdges/range    {14, 1}
  double   stommsMesh/planes/3/physicsClassification/edge/closed/psi                 {13, 1}
  int32_t  stommsMesh/planes/3/physicsClassification/face/core                       {13, 1}
  int32_t  stommsMesh/planes/3/physicsClassification/vertex/criticalPoints/oPoint/1  scalar
  int32_t  stommsMesh/planes/3/rank                                                  {1}
  int8_t   stommsMesh/planes/3/tags/0/class_dim/data                                 {3676, 1}
  int32_t  stommsMesh/planes/3/tags/0/class_id/data                                  {3676, 1}
  double   stommsMesh/planes/3/tags/0/coordinates/data                               {3676, 2}
  int64_t  stommsMesh/planes/3/tags/0/global/data                                    {3676, 1}
  int8_t   stommsMesh/planes/3/tags/1/class_dim/data                                 {10738, 1}
  int32_t  stommsMesh/planes/3/tags/1/class_id/data                                  {10738, 1}
  int64_t  stommsMesh/planes/3/tags/1/global/data                                    {10738, 1}
  int8_t   stommsMesh/planes/3/tags/2/class_dim/data                                 {7063, 1}
  int32_t  stommsMesh/planes/3/tags/2/class_id/data                                  {7063, 1}
  int64_t  stommsMesh/planes/3/tags/2/global/data                                    {7063, 1}
  int32_t  stommsMesh/planes/4/comm_size                                             {1}
  int32_t  stommsMesh/planes/4/dim                                                   {1}
  int32_t  stommsMesh/planes/4/downward_adj/1_to_0                                   {10738, 2}
  int32_t  stommsMesh/planes/4/downward_adj/2_to_1                                   {7063, 3}
  int8_t   stommsMesh/planes/4/downward_codes/2                                      {7063, 3}
  int32_t  stommsMesh/planes/4/family                                                {1}
  int32_t  stommsMesh/planes/4/mesh_version                                          {1}
  int32_t  stommsMesh/planes/4/modelAdj/0/toEdge/range                               {2, 1}
  int32_t  stommsMesh/planes/4/modelAdj/0/toFace/data                                {1, 1}
  int32_t  stommsMesh/planes/4/modelAdj/0/toFace/range                               {2, 1}
  int32_t  stommsMesh/planes/4/modelAdj/0/vertexIds                                  {1, 1}
  int32_t  stommsMesh/planes/4/modelAdj/1/edgeIds                                    {13, 1}
  int32_t  stommsMesh/planes/4/modelAdj/1/toFace/data                                {25, 1}
  int32_t  stommsMesh/planes/4/modelAdj/1/toFace/range                               {14, 1}
  int32_t  stommsMesh/planes/4/modelAdj/1/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/4/modelAdj/2/faceIds                                    {13, 1}
  int32_t  stommsMesh/planes/4/modelAdj/2/toEdge/data                                {25, 1}
  int32_t  stommsMesh/planes/4/modelAdj/2/toEdge/range                               {14, 1}
  int32_t  stommsMesh/planes/4/modelAdj/2/toVertex/data                              {1, 1}
  int32_t  stommsMesh/planes/4/modelAdj/2/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/4/nghost_layers                                         {1}
  int32_t  stommsMesh/planes/4/nverts                                                {1}
  int32_t  stommsMesh/planes/4/parting                                               {1}
  int32_t  stommsMesh/planes/4/physicsClassification/edge/closed/fluxIds             {13, 1}
  int32_t  stommsMesh/planes/4/physicsClassification/edge/closed/modelEdges/data     {13, 1}
  int32_t  stommsMesh/planes/4/physicsClassification/edge/closed/modelEdges/range    {14, 1}
  double   stommsMesh/planes/4/physicsClassification/edge/closed/psi                 {13, 1}
  int32_t  stommsMesh/planes/4/physicsClassification/face/core                       {13, 1}
  int32_t  stommsMesh/planes/4/physicsClassification/vertex/criticalPoints/oPoint/1  scalar
  int32_t  stommsMesh/planes/4/rank                                                  {1}
  int8_t   stommsMesh/planes/4/tags/0/class_dim/data                                 {3676, 1}
  int32_t  stommsMesh/planes/4/tags/0/class_id/data                                  {3676, 1}
  double   stommsMesh/planes/4/tags/0/coordinates/data                               {3676, 2}
  int64_t  stommsMesh/planes/4/tags/0/global/data                                    {3676, 1}
  int8_t   stommsMesh/planes/4/tags/1/class_dim/data                                 {10738, 1}
  int32_t  stommsMesh/planes/4/tags/1/class_id/data                                  {10738, 1}
  int64_t  stommsMesh/planes/4/tags/1/global/data                                    {10738, 1}
  int8_t   stommsMesh/planes/4/tags/2/class_dim/data                                 {7063, 1}
  int32_t  stommsMesh/planes/4/tags/2/class_id/data                                  {7063, 1}
  int64_t  stommsMesh/planes/4/tags/2/global/data                                    {7063, 1}
  int32_t  stommsMesh/planes/5/comm_size                                             {1}
  int32_t  stommsMesh/planes/5/dim                                                   {1}
  int32_t  stommsMesh/planes/5/downward_adj/1_to_0                                   {10738, 2}
  int32_t  stommsMesh/planes/5/downward_adj/2_to_1                                   {7063, 3}
  int8_t   stommsMesh/planes/5/downward_codes/2                                      {7063, 3}
  int32_t  stommsMesh/planes/5/family                                                {1}
  int32_t  stommsMesh/planes/5/mesh_version                                          {1}
  int32_t  stommsMesh/planes/5/modelAdj/0/toEdge/range                               {2, 1}
  int32_t  stommsMesh/planes/5/modelAdj/0/toFace/data                                {1, 1}
  int32_t  stommsMesh/planes/5/modelAdj/0/toFace/range                               {2, 1}
  int32_t  stommsMesh/planes/5/modelAdj/0/vertexIds                                  {1, 1}
  int32_t  stommsMesh/planes/5/modelAdj/1/edgeIds                                    {13, 1}
  int32_t  stommsMesh/planes/5/modelAdj/1/toFace/data                                {25, 1}
  int32_t  stommsMesh/planes/5/modelAdj/1/toFace/range                               {14, 1}
  int32_t  stommsMesh/planes/5/modelAdj/1/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/5/modelAdj/2/faceIds                                    {13, 1}
  int32_t  stommsMesh/planes/5/modelAdj/2/toEdge/data                                {25, 1}
  int32_t  stommsMesh/planes/5/modelAdj/2/toEdge/range                               {14, 1}
  int32_t  stommsMesh/planes/5/modelAdj/2/toVertex/data                              {1, 1}
  int32_t  stommsMesh/planes/5/modelAdj/2/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/5/nghost_layers                                         {1}
  int32_t  stommsMesh/planes/5/nverts                                                {1}
  int32_t  stommsMesh/planes/5/parting                                               {1}
  int32_t  stommsMesh/planes/5/physicsClassification/edge/closed/fluxIds             {13, 1}
  int32_t  stommsMesh/planes/5/physicsClassification/edge/closed/modelEdges/data     {13, 1}
  int32_t  stommsMesh/planes/5/physicsClassification/edge/closed/modelEdges/range    {14, 1}
  double   stommsMesh/planes/5/physicsClassification/edge/closed/psi                 {13, 1}
  int32_t  stommsMesh/planes/5/physicsClassification/face/core                       {13, 1}
  int32_t  stommsMesh/planes/5/physicsClassification/vertex/criticalPoints/oPoint/1  scalar
  int32_t  stommsMesh/planes/5/rank                                                  {1}
  int8_t   stommsMesh/planes/5/tags/0/class_dim/data                                 {3676, 1}
  int32_t  stommsMesh/planes/5/tags/0/class_id/data                                  {3676, 1}
  double   stommsMesh/planes/5/tags/0/coordinates/data                               {3676, 2}
  int64_t  stommsMesh/planes/5/tags/0/global/data                                    {3676, 1}
  int8_t   stommsMesh/planes/5/tags/1/class_dim/data                                 {10738, 1}
  int32_t  stommsMesh/planes/5/tags/1/class_id/data                                  {10738, 1}
  int64_t  stommsMesh/planes/5/tags/1/global/data                                    {10738, 1}
  int8_t   stommsMesh/planes/5/tags/2/class_dim/data                                 {7063, 1}
  int32_t  stommsMesh/planes/5/tags/2/class_id/data                                  {7063, 1}
  int64_t  stommsMesh/planes/5/tags/2/global/data                                    {7063, 1}
  int32_t  stommsMesh/planes/6/comm_size                                             {1}
  int32_t  stommsMesh/planes/6/dim                                                   {1}
  int32_t  stommsMesh/planes/6/downward_adj/1_to_0                                   {10738, 2}
  int32_t  stommsMesh/planes/6/downward_adj/2_to_1                                   {7063, 3}
  int8_t   stommsMesh/planes/6/downward_codes/2                                      {7063, 3}
  int32_t  stommsMesh/planes/6/family                                                {1}
  int32_t  stommsMesh/planes/6/mesh_version                                          {1}
  int32_t  stommsMesh/planes/6/modelAdj/0/toEdge/range                               {2, 1}
  int32_t  stommsMesh/planes/6/modelAdj/0/toFace/data                                {1, 1}
  int32_t  stommsMesh/planes/6/modelAdj/0/toFace/range                               {2, 1}
  int32_t  stommsMesh/planes/6/modelAdj/0/vertexIds                                  {1, 1}
  int32_t  stommsMesh/planes/6/modelAdj/1/edgeIds                                    {13, 1}
  int32_t  stommsMesh/planes/6/modelAdj/1/toFace/data                                {25, 1}
  int32_t  stommsMesh/planes/6/modelAdj/1/toFace/range                               {14, 1}
  int32_t  stommsMesh/planes/6/modelAdj/1/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/6/modelAdj/2/faceIds                                    {13, 1}
  int32_t  stommsMesh/planes/6/modelAdj/2/toEdge/data                                {25, 1}
  int32_t  stommsMesh/planes/6/modelAdj/2/toEdge/range                               {14, 1}
  int32_t  stommsMesh/planes/6/modelAdj/2/toVertex/data                              {1, 1}
  int32_t  stommsMesh/planes/6/modelAdj/2/toVertex/range                             {14, 1}
  int32_t  stommsMesh/planes/6/nghost_layers                                         {1}
  int32_t  stommsMesh/planes/6/nverts                                                {1}
  int32_t  stommsMesh/planes/6/parting                                               {1}
  int32_t  stommsMesh/planes/6/physicsClassification/edge/closed/fluxIds             {13, 1}
  int32_t  stommsMesh/planes/6/physicsClassification/edge/closed/modelEdges/data     {13, 1}
  int32_t  stommsMesh/planes/6/physicsClassification/edge/closed/modelEdges/range    {14, 1}
  double   stommsMesh/planes/6/physicsClassification/edge/closed/psi                 {13, 1}
  int32_t  stommsMesh/planes/6/physicsClassification/face/core                       {13, 1}
  int32_t  stommsMesh/planes/6/physicsClassification/vertex/criticalPoints/oPoint/1  scalar
  int32_t  stommsMesh/planes/6/rank                                                  {1}
  int8_t   stommsMesh/planes/6/tags/0/class_dim/data                                 {3676, 1}
  int32_t  stommsMesh/planes/6/tags/0/class_id/data                                  {3676, 1}
  double   stommsMesh/planes/6/tags/0/coordinates/data                               {3676, 2}
  int64_t  stommsMesh/planes/6/tags/0/global/data                                    {3676, 1}
  int8_t   stommsMesh/planes/6/tags/1/class_dim/data                                 {10738, 1}
  int32_t  stommsMesh/planes/6/tags/1/class_id/data                                  {10738, 1}
  int64_t  stommsMesh/planes/6/tags/1/global/data                                    {10738, 1}
  int8_t   stommsMesh/planes/6/tags/2/class_dim/data                                 {7063, 1}
  int32_t  stommsMesh/planes/6/tags/2/class_id/data                                  {7063, 1}
  int64_t  stommsMesh/planes/6/tags/2/global/data                                    {7063, 1}


Sample Adios2 output file for single plane with background eqdsk information
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: text

  string   StommsVersion                                                               scalar
  double   stommsMesh/fieldGridData/bicubicSplineCoefficients                          {64, 64, 16}
  double   stommsMesh/fieldGridData/boundaryPointsR                                    {90, 1}
  double   stommsMesh/fieldGridData/boundaryPointsZ                                    {90, 1}
  double   stommsMesh/fieldGridData/domainBox                                          {4, 1}
  double   stommsMesh/fieldGridData/gridPointsR                                        {65, 1}
  double   stommsMesh/fieldGridData/gridPointsZ                                        {65, 1}
  double   stommsMesh/fieldGridData/limiterPointsR                                     {49, 1}
  double   stommsMesh/fieldGridData/limiterPointsZ                                     {49, 1}
  double   stommsMesh/fieldGridData/poloidalCurrent                                    {65, 1}
  double   stommsMesh/fieldGridData/poloidalCurrentSplineCoefficients                  {2, 65}
  double   stommsMesh/fieldGridData/psi                                                {65, 1}
  double   stommsMesh/fieldGridData/psiAtChebyshevPoints                               {64, 64, 16}
  double   stommsMesh/fieldGridData/psiGrid                                            {65, 65}
  double   stommsMesh/fieldGridData/psiSplineCoefficients                              {4, 65, 65}
  int32_t  stommsMesh/planes/0/comm_size                                               {1}
  int32_t  stommsMesh/planes/0/dim                                                     {1}
  int32_t  stommsMesh/planes/0/downward_adj/1_to_0                                     {6969, 2}
  int32_t  stommsMesh/planes/0/downward_adj/2_to_1                                     {4603, 3}
  int8_t   stommsMesh/planes/0/downward_codes/2                                        {4603, 3}
  int32_t  stommsMesh/planes/0/family                                                  {1}
  int32_t  stommsMesh/planes/0/mesh_version                                            {1}
  int32_t  stommsMesh/planes/0/modelAdj/0/toEdge/data                                  {130, 1}
  int32_t  stommsMesh/planes/0/modelAdj/0/toEdge/range                                 {67, 1}
  int32_t  stommsMesh/planes/0/modelAdj/0/toFace/data                                  {93, 1}
  int32_t  stommsMesh/planes/0/modelAdj/0/toFace/range                                 {67, 1}
  int32_t  stommsMesh/planes/0/modelAdj/0/vertexIds                                    {66, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/edgeIds                                      {72, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/toFace/data                                  {93, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/toFace/range                                 {73, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/toVertex/data                                {130, 1}
  int32_t  stommsMesh/planes/0/modelAdj/1/toVertex/range                               {73, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/faceIds                                      {21, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/toEdge/data                                  {93, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/toEdge/range                                 {22, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/toVertex/data                                {93, 1}
  int32_t  stommsMesh/planes/0/modelAdj/2/toVertex/range                               {22, 1}
  int32_t  stommsMesh/planes/0/nghost_layers                                           {1}
  int32_t  stommsMesh/planes/0/nverts                                                  {1}
  int32_t  stommsMesh/planes/0/parting                                                 {1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/closed/fluxIds               {13, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/closed/modelEdges/data       {13, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/closed/modelEdges/range      {14, 1}
  double   stommsMesh/planes/0/physicsClassification/edge/closed/psi                   {13, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/open/fluxIds                 {5, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/open/modelEdges/data         {5, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/open/modelEdges/range        {6, 1}
  double   stommsMesh/planes/0/physicsClassification/edge/open/psi                     {5, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/separatrix/fluxIds           {1, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/separatrix/modelEdges/data   {3, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/separatrix/modelEdges/range  {2, 1}
  double   stommsMesh/planes/0/physicsClassification/edge/separatrix/psi               {1, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/wall/fluxIds                 {1, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/wall/modelEdges/data         {51, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/edge/wall/modelEdges/range        {2, 1}
  double   stommsMesh/planes/0/physicsClassification/edge/wall/psi                     {1, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/face/core                         {14, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/face/nearVacuumRegion             {3, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/face/privateRegion                {3, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/face/sol                          {1, 1}
  int32_t  stommsMesh/planes/0/physicsClassification/vertex/criticalPoints/oPoint/1    scalar
  int32_t  stommsMesh/planes/0/physicsClassification/vertex/criticalPoints/xPoint/1    scalar
  int32_t  stommsMesh/planes/0/rank                                                    {1}
  int8_t   stommsMesh/planes/0/tags/0/class_dim/data                                   {2367, 1}
  int32_t  stommsMesh/planes/0/tags/0/class_id/data                                    {2367, 1}
  double   stommsMesh/planes/0/tags/0/coordinates/data                                 {2367, 2}
  int64_t  stommsMesh/planes/0/tags/0/global/data                                      {2367, 1}
  int8_t   stommsMesh/planes/0/tags/1/class_dim/data                                   {6969, 1}
  int32_t  stommsMesh/planes/0/tags/1/class_id/data                                    {6969, 1}
  int64_t  stommsMesh/planes/0/tags/1/global/data                                      {6969, 1}
  int8_t   stommsMesh/planes/0/tags/2/class_dim/data                                   {4603, 1}
  int32_t  stommsMesh/planes/0/tags/2/class_id/data                                    {4603, 1}
  int64_t  stommsMesh/planes/0/tags/2/global/data                                      {4603, 1}
