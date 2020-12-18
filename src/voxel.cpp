#include "voxel.h"
#include "misc.h"
#include "atom.h"
//#include "atomtree.h"
#include <cmath> // abs, pow
#include <cassert>

///////////////////
// AUX FUNCTIONS //
///////////////////

inline double Voxel::calcRadiusOfInfluence(const double& max_depth){
  return max_depth != 0 ? 0.86602540378 * _grid_size * (pow(2,max_depth) - 1) : 0;
}

bool allAtomsClose(
    const double&, const std::array<double,3>&, const std::array<Vector,3>&, char); 

/////////////////
// CONSTRUCTOR //
/////////////////

Voxel::Voxel(){
  type = 'e';
}

////////////
// ACCESS //
////////////

Voxel& Voxel::access(const short& x, const short& y, const short& z){
  assert(x*y*z < 8);
  return data[4 * z + 2 * y + x];
}

Voxel& Voxel::access(const short& i){
  assert(i <= 8);
  return data[i];
}

char Voxel::getType(){
  return type;
}

void Voxel::setType(char input){
  type = input;
}

//////////////
// SET TYPE //
//////////////

void Voxel::storeUniversal(AtomTree atomtree, double grid_size, double r_probe1){
  _atomtree = atomtree;
  _grid_size = grid_size;
  _r_probe1 = r_probe1;
}

char Voxel::determineType(std::array<double,3> vxl_pos, const double max_depth)
{
  double r_vxl = calcRadiusOfInfluence(max_depth); // calculated every time, since max_depth may change
  
  double rad_max = _atomtree.getMaxRad();
  std::vector<Atom> very_close_atoms = listFromTree(_atomtree.getRoot(), vxl_pos, r_vxl, rad_max, 0, 0);
 
  { // TODO: FUNCTION?
    // can be further optimised: use traverTree function instead
    // 2nd optimisation: higher level voxels with radii larger than the atoms will never be type 'a'. in
    // these cases the function can abort once a type 'm' has been found
    // is voxel inside an atom?
    for (Atom atom : very_close_atoms){ 
      isAtom(atom, distance(vxl_pos, atom.getPos()), r_vxl);
      if (type=='a'){return type;}
    }
  }

  // probe mode
  
  { // TODO: FUNCTION?
    /*
    Atom probe_atom = Atom(); // initialise with empty atom
    if (very_close_atoms.size() > 0) {probe_atom = very_close_atoms[0];}
    else {
      // TODO: just for testing, this is inefficient. write a function that returns the first atom close enough
      std::vector<Atom> close_atoms = listFromTree(_atomtree.getRoot(), vxl_pos, 0, rad_max, _r_probe1, 0);
      // probe_atom = firstFromTree(_atomtree.getRoot(), vxl_pos, 0, rad_max, _r_probe1, 0);
      if (close_atoms.size()>0){
        probe_atom = close_atoms[0];
      }
    }
    */
  
    // pass _r_probe1 as proper argument, so that this routine may be reused for two probe mode
    std::vector<Atom> close_atoms = listFromTree(_atomtree.getRoot(), vxl_pos, 0, rad_max, _r_probe1*2, 0);
    isProbeExcluded(vxl_pos, _r_probe1, r_vxl, close_atoms);
    if (type=='x'){return type;}
    /*
    if (probe_atom.isValid()){ // there is an atom near the voxel
      bool accessibility_checked = false; // not necessary when only passing one atom
      isProbeExcluded(probe_atom, vxl_pos, _r_probe1, r_vxl, close_atoms);
    }
    */
  }
  
  // probe mode
  
  if(type == 'm'){
    splitVoxel(vxl_pos, max_depth);
  }
	return type;
}

void Voxel::splitVoxel(const std::array<double,3>& vxl_pos, const double& max_depth){
  // split into 8 subvoxels
	short resultcount = 0;
  for(int i = 0; i < 8; i++){
    data.push_back(Voxel());
    // modify position
    std::array<int,3> factors = {
      (i%2) >= 1 ? 1 : -1,
      (i%4) >= 2 ? 1 : -1,
       i    >= 4 ? 1 : -1};
   
    std::array<double,3> new_pos;
    for(int dim = 0; dim < 3; dim++){
      new_pos[dim] = vxl_pos[dim] + factors[dim] * _grid_size * std::pow(2,max_depth-2);//why -2?
    }
    resultcount += data[i].determineType(new_pos, max_depth-1);
  }
	//determine if all children have the same type
  // TODO: delete data vector in this case
	if (resultcount == 'a'*8){
	  type = 'a';
	} else if (resultcount == 'e'*8) {
	  type = 'e';
	}
  //
}

// go through a tree, starting from node. return a list of atoms that are a specified max distance (max_dist)
// from a point with radius rad_point.
std::vector<Atom> Voxel::listFromTree(
  const AtomNode* node, 
  const std::array<double,3>& pos_point, // consider using custom vector class instead
  const double& rad_point, 
  const double& rad_max,
  const double& max_dist=0, 
  const char dim=0)
{
  std::vector<Atom> atom_list;
  if (node == NULL){return atom_list;}
  
  
  // distance between atom and point along one dimension
  double dist1D = distance(node->atom->getPos(), pos_point, dim);
  double rad_atom = node->atom->getRad();
  
  std::vector<Atom> temp;
  if (abs(dist1D) > rad_point + rad_max + max_dist) { // then atom is too far
      temp = listFromTree(dist1D < 0 ? node->left_child : node->right_child, pos_point, rad_point, rad_max, max_dist, (dim+1)%3);
      atom_list.insert(atom_list.end(), temp.begin(), temp.end());
  } 
  else { // then atom may be close enough
    if (distance(node->atom->getPos(), pos_point) < rad_point + rad_atom + max_dist){
      atom_list.push_back(*(node->atom));
    }
    
    // continue with both children
    for (AtomNode* child : {node->left_child, node->right_child}){
      temp = listFromTree(child, pos_point, rad_point, rad_max, max_dist, (dim+1)%3);
      atom_list.insert(atom_list.end(), temp.begin(), temp.end());
    }
  }
  return atom_list;
}

/* don' delete, could become useful later
// use the properties of the binary tree to recursively traverse the tree and 
// only check the voxel type with respect to relevant atoms. at the end of this 
// method, the type of the voxel will have been set.
void Voxel::traverseTree
  (const AtomNode* node, 
   int dim, const double& at_rad, 
   const double& vxl_rad, 
   const std::array<double,3> vxl_pos, 
   const double& max_depth,
   bool& accessibility_checked){

  if (node == NULL){return;}
  // distance between atom and voxel along one dimension
  double dist1D = distance(node->atom->getPos(), vxl_pos, dim);
 
  // TODO: this condition is not optimised. _r_probe1 might be unneccessary here and slow down the algo
  if (abs(dist1D) > (vxl_rad + at_rad + _r_probe1)){ // then atom is too far to matter for voxel type
      traverseTree(dist1D < 0 ? node->left_child : node->right_child,
				   (dim+1)%3, at_rad, vxl_rad, vxl_pos, max_depth, accessibility_checked);
  } else{ // then atom is close enough to influence voxel type
    // evaluate voxel type with respect to the found atom
    determineTypeSingleAtom(*(node->atom), vxl_pos, max_depth, accessibility_checked);
    // continue with both children
    for (AtomNode* child : {node->left_child, node->right_child}){
      traverseTree(child, (dim+1)%3, at_rad, vxl_rad, vxl_pos, max_depth, accessibility_checked);
    }
  }
}
*/

// no longer used
/*
void Voxel::determineTypeSingleAtom(const Atom& atom, 
                                    std::array<double,3> vxl_pos, // voxel centre
                                    const double max_depth,
                                    bool& accessibility_checked){


  if(type == 'a'){return;} // type 'a' can never be changed

  double dist_vxl_at = distance(vxl_pos, atom.getPos());

  // if bottom level voxel: radius of influence = 0, i.e., treat like point
  // if higher level voxel: radius of influence > 0
  double radius_of_influence = 
    max_depth != 0 ? 0.86602540378 * _grid_size * (pow(2,max_depth) - 1) : 0;
  // I believe the use of a conditional makes the code slightly faster -JM
  
  // is voxel inside atom? 
  if (isAtom(atom, dist_vxl_at, radius_of_influence)){return;}
  
}
*/

////////////////////
// CHECK FOR TYPE //
////////////////////

bool Voxel::isAtom(const Atom& atom, const double& dist_vxl_at, const double& radius_of_influence){
  if(atom.rad > (dist_vxl_at + radius_of_influence)){ 
    setType('a'); // in atom
    return true;
  }
  else if(atom.rad >= (dist_vxl_at - radius_of_influence)){
    setType('m'); // mixed
    return true;
  }
  return false;
}

bool Voxel::isProbeExcluded(const std::array<double,3>& vxl_pos, const double& r_probe, const double& radius_of_influence, const std::vector<Atom>& close_atoms){ 
  
  if(type == 'm'){return false;} // type 'm' can never be changed by probe
  
  for (int i = 0; i < close_atoms.size(); i++){
    Atom atom1 = close_atoms[i];
    // for simplicity all vectors are shifted by -vec_offset, so that the atom is in the origin
    Vector vec_offset = Vector(atom1.getPos());
    
    std::array<double,3> atom_radii;
    std::array<Vector,3> vectors;
    
    vectors[0] = Vector();
    atom_radii[0] = atom1.getRad();
  
    Vector vec_vxl = Vector(vxl_pos) - vec_offset;
    
    for (int j = i+1; j < close_atoms.size(); j++){
      Atom atom2 = close_atoms[j];
      atom_radii[1] = atom2.getRad();
      vectors[1] = Vector(atom2.getPos()) - vec_offset;
  
      if (!allAtomsClose(r_probe, atom_radii, vectors, 2)){continue;}
      if (isExcludedByPair(vec_vxl, vectors[1], atom_radii[0], atom_radii[1], r_probe, radius_of_influence)){return true;}
      
      for (int k = j+1; k < close_atoms.size(); k++){
        Atom atom3 = close_atoms[k];
        atom_radii[2] = atom3.getRad();
        vectors[2] = Vector(atom3.getPos()) - vec_offset;
        
        if (!allAtomsClose(r_probe, atom_radii, vectors, 3)){continue;}
        if (isExcludedByTriplet(vec_vxl, radius_of_influence, vectors, atom_radii, r_probe)){return true;}
  
      }
    }
  }
  return false;
}

void breakpoint(){return;}

bool Voxel::isExcludedByTriplet(
  const Vector& vec_vxl, 
  const double& rad_vxl, 
  const std::array<Vector,3>& vec_atom,
  const std::array<double,3>& rad_atom,
  const double& rad_probe){
  // check if between atom1 and atom2 - done by isExcludedByPair
//  double dist_vxl_12 = unitvec_12 * vec_vxl; // voxel vector component along 12

  Vector unitvec_13 = vec_atom[2].normalise(); // vector pointing from atom1 to atom3
  double dist_vxl_13 = unitvec_13 * vec_vxl; // voxel vector component along 13
  if (dist_vxl_13 > 0 && vec_atom[2] > dist_vxl_13){
      //2 * rad_probe + rad_atom[0] + rad_atom[2]){ // check if between atom1 and atom3
    
    double dist_12 = vec_atom[1].length();
    double dist_probe_12 = (rad_atom[0]-rad_atom[1])*(((rad_atom[0] + rad_atom[1]) + 2*rad_probe)/(2*dist_12)) + (dist_12/2);
//        pow(rad_atom[0]+rad_probe,2) - pow(rad_atom[1]+rad_probe,2))/(2*dist_12);
    
    double dist_13 = vec_atom[2].length();
    double dist_probe_13 = (rad_atom[0]-rad_atom[2])*(2*rad_probe + (rad_atom[0]+rad_atom[2]))/(2*dist_13) + (dist_13/2);

    Vector unitvec_12 = vec_atom[1].normalise(); // vector pointing from atom1 to atom2 // calculated in Pairs
    //Vector vec_probe_plane = (unitvec_12*dist_probe_12 + unitvec_13*dist_probe_13)/2;
    
    Vector vec_probe_plane;
    {
      Vector vec_probe_12 = dist_probe_12 * unitvec_12;
      Vector vec_probe_13 = dist_probe_13 * unitvec_13;

      Vector vec_normal_12 = crossproduct(crossproduct(vec_probe_12,vec_probe_13),vec_probe_12);
      Vector vec_normal_13 = crossproduct(crossproduct(vec_probe_12,vec_probe_13),vec_probe_13);

      double c1 = ((vec_probe_13[1]-vec_probe_12[1]) + (vec_normal_13[1]/vec_normal_13[0])*(vec_probe_12[0]-vec_probe_13[0]))/(vec_normal_12[1]-(vec_normal_12[0]*vec_normal_13[1]/vec_normal_13[0]));

      vec_probe_plane = vec_probe_12 + (c1 * vec_normal_12);
    }
    
    Vector unitvec_normal = crossproduct(unitvec_12,unitvec_13).normalise();
    unitvec_normal = unitvec_normal * ( signbit(unitvec_normal*vec_vxl)? -1 : 1 ); // let unitvec normal point towards vxl

    Vector vec_probe_normal = unitvec_normal * pow((pow(rad_atom[0]+rad_probe,2) - vec_probe_plane*vec_probe_plane),0.5);
    Vector vec_probe = vec_probe_plane + vec_probe_normal;
   
    Vector unitvec_1p = vec_probe.normalise();

    // check whether vxl is inside tetrahedron spanned by atoms1-3 and probe 
    {
      std::array<Vector,4> normals = 
        {crossproduct(unitvec_12, unitvec_13), // normal 1-2-3 plane
         crossproduct(unitvec_13, unitvec_1p), // normal 1-3-p plane
         crossproduct(unitvec_1p, unitvec_12), // normal 1-p-2 plane
         crossproduct(vec_probe-vec_atom[1], vec_atom[2]-vec_atom[1])}; // normal 2-p-3 plane
      
      bool sign;
      for (char i = 0; i < normals.size(); i++){
        if (!i){ // for first iteration save sign
          double direction = (vec_vxl*normals[i]);
          sign = signbit(vec_vxl*normals[i]);
        }
        else { // if sign ever differs then vxl is not inside tetrahedron
          double direction = (vec_vxl-vec_atom[i-1])*normals[i];
          if (sign != signbit((vec_vxl-vec_atom[i-1])*normals[i])){
            return false;  
          }
        }
      }
    }
    // function
    // this block should be made a function since it is also used in pairs
    if (vec_probe-vec_vxl > (rad_probe+rad_vxl)){ // then all subvoxels are inaccessible
      setType('x');
      return true;
    }
    else if (vec_probe-vec_vxl <= (rad_probe-rad_vxl)){ // then all subvoxels are accessible
      return false;
    }
    else { // then each subvoxel has to be evaluated
      setType('m');
      return false;
    }
    // function
  }
  return false;
}

bool Voxel::isExcludedByPair(const Vector& vec_vxl, const Vector& vec_atat, const double& rad_atom1, const double& rad_atom2, const double& rad_probe, const double& rad_vxl){
    
  Vector unitvec_parallel = vec_atat.normalise();
  double vxl_parallel = vec_vxl * unitvec_parallel; 
  if (vxl_parallel > 0 && vec_atat > vxl_parallel){ // then voxel is between atoms
    
    Vector unitvec_orthogonal = (vec_vxl-unitvec_parallel*vxl_parallel).normalise();
    double vxl_orthogonal = vec_vxl * unitvec_orthogonal; 
    
    double dist_atom1_probe = rad_atom1 + rad_probe;
    double dist_atom2_probe = rad_atom2 + rad_probe;
    double dist_atom1_atom2 = vec_atat.length();

    double angle_atom1 = acos((pow(dist_atom1_probe,2) + pow(dist_atom1_atom2,2) - pow(dist_atom2_probe,2))/(2*dist_atom1_probe*dist_atom1_atom2));
    double angle_vxl1 = atan(vxl_orthogonal/vxl_parallel);

    if (angle_atom1 > angle_vxl1){
      double angle_atom2 = acos((pow(dist_atom2_probe,2) + pow(dist_atom1_atom2,2) - pow(dist_atom1_probe,2))/(2*dist_atom2_probe*dist_atom1_atom2));
      double angle_vxl2 = atan(vxl_orthogonal/(dist_atom1_atom2-vxl_parallel));
      if (angle_atom2 > angle_vxl2){ // then voxel is in triangle spanned by atoms and probe
        double probe_parallel = ((pow(dist_atom1_probe,2) + pow(dist_atom1_atom2,2) - pow(dist_atom2_probe,2))/(2*dist_atom1_atom2));
        double probe_orthogonal = pow(pow(dist_atom1_probe,2)-pow(probe_parallel,2),0.5);
        
        Vector vec_probe = probe_parallel * unitvec_parallel + probe_orthogonal * unitvec_orthogonal;
        
        if (vec_probe-vec_vxl > rad_probe+rad_vxl){ // then all subvoxels are inaccessible
          setType('x');
          return true;
        }
        else if (vec_probe-vec_vxl <= rad_probe-rad_vxl){ // then all subvoxels are accessible
          return false;
        }
        else { // then each subvoxel has to be evaluated
          setType('m');
          return false;
        }
      }
    }
  }
  return false;
}

///////////
// TALLY //
///////////

// TODO: Optimise. Allow for tallying multiples types at once
size_t Voxel::tallyVoxelsOfType(const char volume_type, const int max_depth){
  // if voxel is of type "mixed" (i.e. data vector is not empty)
  if(!data.empty()){
    // then total number of voxels is given by tallying all subvoxels
    size_t total = 0;
    for(int i = 0; i < 8; i++){
      total += data[i].tallyVoxelsOfType(volume_type, max_depth-1);
    }
    return total;
  }
  // if voxel is of the type of interest, tally the voxel in units of bottom level voxels
  else if(type == volume_type){
    return pow(pow(2,max_depth),3); // return number of bottom level voxels
  }
  // if neither empty nor of the type of interest, then the voxel doesn't count towards the total
  return 0;
}

//////////////////////////////
// AUX FUNCTION DEFINITIONS //
//////////////////////////////

bool allAtomsClose(
    const double& r_probe, 
    const std::array<double,3>& atom_radii, 
    const std::array<Vector,3>& vectors, 
    char n_atoms) 
{
  bool all_atoms_close = true;

  Vector last_added = vectors[n_atoms-1];
  double rad_last_added = atom_radii[n_atoms-1];
  for (char i = 0; i < n_atoms-1; i++){
    if (vectors[i]-last_added > 2*r_probe + atom_radii[i] + rad_last_added){
      return false;
    }
  }
  return true; // if all atoms close
}

