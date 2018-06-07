//
// An OpenInventor object to represent an ideal track and ideal beam track
//
#include <HEPVis/nodekits/SoIdealTrackKit.h>

//used in the kits
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
// Nurb Curves
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h>

#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCube.h>

#include <math.h>
#include <float.h>

#include <iostream>

inline double copy_sign (double x, double y) {
  return (fabs(x) * y /fabs(y));
}

// initialise constants
  const double  SoIdealTrackKit::TWOPI = 2*M_PI;
  const int     SoIdealTrackKit::NORDER = 3;
  const double  SoIdealTrackKit::SPEEDOLIGHT = 0.00299792458; // in (m/nsec) *10^-2
        double  SoIdealTrackKit::rmax = 1.40;      //max radius in central tracker
        double  SoIdealTrackKit::zmax = 3.2;       //max z extent
        double  SoIdealTrackKit::bfield = 4.0;	 //B Field in Tesla
// Ideal track
SO_KIT_SOURCE(SoIdealTrackKit)

void SoIdealTrackKit::initClass()
{
        SO_KIT_INIT_CLASS(SoIdealTrackKit, SoShapeKit, "ShapeKit");
		if (ParticleChar::getByName("unknown") == NULL) {
			ParticleChar::initParticles();}
}

SoIdealTrackKit::SoIdealTrackKit()
{
        SO_KIT_CONSTRUCTOR(SoIdealTrackKit);
        
        SO_KIT_CHANGE_ENTRY_TYPE(shape, SoShape, SoNurbsCurve);
        SO_KIT_CHANGE_NULL_BY_DEFAULT(coordinate4,FALSE);
        SO_KIT_CHANGE_NULL_BY_DEFAULT(appearance,FALSE);
        
        SO_KIT_ADD_FIELD(Vertex, (0, 0, 0));
        SO_KIT_ADD_FIELD(Phi, (0));
        SO_KIT_ADD_FIELD(Radius, (1));
        SO_KIT_ADD_FIELD(Zeta, (0.01F));
        SO_KIT_ADD_FIELD(T0, (0));
        SO_KIT_ADD_FIELD(DeltaT, (1));
		SO_KIT_ADD_FIELD(T_0,(0));
		SO_KIT_ADD_FIELD(T_1,(500));
		SO_KIT_ADD_FIELD(ParticleType,("unknown"));
		SO_KIT_ADD_FIELD(alternateRep, (NULL));

        
        SO_KIT_ADD_CATALOG_ENTRY(debugPoints,SoShapeKit,TRUE,topSeparator,\x0,TRUE);
        
        SO_KIT_INIT_INSTANCE();
        
        fieldSensorT0 = new SoFieldSensor(SoIdealTrackKit::fieldsChangedCB,(void *) this);
        fieldSensorDeltaT = new SoFieldSensor(SoIdealTrackKit::fieldsChangedCB,(void *) this);
        fieldSensorT0->setPriority(0);
        fieldSensorDeltaT->setPriority(0);
        
        setUpConnections(TRUE,TRUE);
}
void SoIdealTrackKit::print_me()const {
	SbName trackName = getName();
	if ( trackName != SbName("")) {
		std::cout << trackName.getString() << std::endl;
	}
	std::cout << "Particle: " << particleChar->getName() << std::endl;
	std::cout << "Transverse Momentum: " << pt << "GeV" << std::endl;
        std::cout << "Radius of curvature: " << Radius.getValue() << "m" << std::endl;
        std::cout << "Zeta: " << Zeta.getValue() << std::endl;
        SbVec3f vertex = Vertex.getValue();
        std::cout << "Vertex (x/y/z in m): "<<  vertex[0] <<", "<< vertex[1] <<", " << vertex[2] << std::endl;
        std::cout << "Phi: " << Phi.getValue() << std::endl;
	std::cout << "T0:       " << T0.getValue() << std::endl;
    	std::cout << "DeltaT is " << DeltaT.getValue() << std::endl;
}
SoIdealTrackKit::~SoIdealTrackKit()
{
	setUpConnections(FALSE,TRUE);
	if (fieldSensorT0 != NULL) {
		delete fieldSensorT0; }
	if (fieldSensorDeltaT != NULL) {
		delete fieldSensorDeltaT;}
}
void SoIdealTrackKit::set_bfield(float bf){
        bfield = bf;
}
void SoIdealTrackKit::set_rExtent(float rExtent){
	rmax = rExtent;
}
void SoIdealTrackKit::set_zExtent(float zExtent){
	zmax = zExtent;
}
//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool SoIdealTrackKit::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;
    if ( onOff ) {
	// We connect AFTER base class.
	SoSeparatorKit::setUpConnections( onOff, doItAlways );
	// Turn sensors on...
	if (fieldSensorT0->getAttachedField() != &this->T0)
	    fieldSensorT0->attach(&this->T0);
	if (fieldSensorDeltaT->getAttachedField() != &this->DeltaT)
	    fieldSensorDeltaT->attach(&this->DeltaT);
//	updateTrack();
    }
    else {
	// We disconnect BEFORE base class.
	if (fieldSensorT0->getAttachedField() != NULL)
	    fieldSensorT0->detach();
	if (fieldSensorDeltaT->getAttachedField() != NULL)
	    fieldSensorDeltaT->detach();
	SoSeparatorKit::setUpConnections( onOff, doItAlways );
    }
    return !(connectionsSetUp = onOff);
}
void SoIdealTrackKit::fieldsChangedCB(void *data, SoSensor *)
{
// Declare the variables needed
    SoIdealTrackKit *k = (SoIdealTrackKit *)data;
    k->updateTrack();
}
float SoIdealTrackKit::time_to_angle(float time){
    float phi = Phi.getValue();
    float radius = Radius.getValue();
    float mass = particleChar->getMass();
    float betat = pt / sqrt( mass*mass + ptot*ptot );
    if (fabs(radius) != 0.0) {
      return float(phi - betat*SPEEDOLIGHT*(time-t_0)/radius);
    } else {
      return phi;
    }
}
float SoIdealTrackKit::angle_to_time(float angle){
    float phi = Phi.getValue();
    float radius = Radius.getValue();
    float mass = particleChar->getMass();
    float betat = pt / sqrt( mass*mass + ptot*ptot );
    //float test = t_0 - (radius * (angle-phi))/(betat*SPEEDOLIGHT);
	return float(t_0 - (radius * (angle-phi))/(betat*SPEEDOLIGHT));
}
float SoIdealTrackKit::z_to_time(float z){
    SbVec3f vertex = Vertex.getValue();    
    float mass = particleChar->getMass();
    float zeta = Zeta.getValue();
    float betaz = float((zeta * SPEEDOLIGHT * 100 * bfield *charge)/ sqrt( mass*mass + ptot*ptot ));
    if (fabs(betaz) != 0.0) {
      return float(t_0 + (z-vertex[2])/(betaz*SPEEDOLIGHT));
    } else {
      return t_0;
    }
}
float SoIdealTrackKit::time_to_z(float time){
    SbVec3f vertex = Vertex.getValue();    
    float mass = particleChar->getMass();
    float zeta = Zeta.getValue();
    float betaz = float((zeta * SPEEDOLIGHT * 100 * bfield * charge)/ sqrt( mass*mass + ptot*ptot ));
    return float((time-t_0) * betaz * SPEEDOLIGHT + vertex[2]);
}
SbVec2f SoIdealTrackKit::time_to_xy(float time){
    float angle = time_to_angle(time);
    SbVec3f vertex = Vertex.getValue();
    double radius = Radius.getValue();
    double phi = Phi.getValue(); 
    double x_c = vertex[0] + radius * sin(phi);
    double y_c = vertex[1] - radius * cos(phi);
    return SbVec2f(float(x_c - (radius) * sin(angle)),
                   float(y_c + (radius) * cos(angle)));
}
void SoIdealTrackKit::initTrack(double vx, double vy, double vz, double px, double py, 
                  double pz, float t0, ParticleChar *p_code)
{
    setUpConnections(FALSE,TRUE);

    particleChar = p_code;
	charge = particleChar->getCharge();
	ParticleType.setValue(particleChar->getName());

    SbVec3f vec3 = SbVec3f((float)px,(float)py,(float)pz);
    ptot = vec3.length();
    SbVec2f vec2 = SbVec2f((float)px,(float)py);
    pt = vec2.length();
    double radius = pt / (SPEEDOLIGHT * 100 * bfield * charge );
    double zeta = pz / (SPEEDOLIGHT * 100 * bfield * charge);
    double phi;
    if (py == 0.0 && px == 0.0) {
      phi = 0.0; }
    else {
      phi = atan2( py, px );
      }
        
    Vertex.setValue((float)vx,(float)vy,(float)vz);
    Radius.setValue((float)radius);
    Zeta.setValue((float)zeta);
    Phi.setValue((float)phi);
    T0.setValue((float)t0);
	T_0.setValue(t0);
    t_0 = t0;
    setPart("material",particleChar->getMaterial());
    setPart("style",particleChar->getStyle());
    set("complexity","0.6");
        
    init_endpts();    
    setUpConnections(TRUE,TRUE);
    
}
void SoIdealTrackKit::init_endpts(){
    double x1,y1,z1=0,del_phi;  //end pt of track & total arc length in x-y plane
    
    float radius = Radius.getValue();
    float zeta = Zeta.getValue();
    SbVec3f vertex = Vertex.getValue();
    float phi = Phi.getValue();
    
    double x_c = vertex[0] + radius * sin(phi);
    double y_c = vertex[1] - radius * cos(phi);
    
	/*  See if there is an intersection between our helix and the visible edge boundary cylinder */
	if (x_c == 0 && y_c == 0 ) {  //concentric with z-axis
		if (fabs(radius) < rmax ) { //inside max radius
			x1 = vertex[0];
			y1 = vertex[1];
			del_phi = - 2. * copy_sign(1.0,charge) * TWOPI;
			if (pt == 0.0) {	  
				z1 = copy_sign(zmax,zeta * (SPEEDOLIGHT * 100.0 * bfield *copy_sign(1.0,charge))); //ie, pz
				del_phi = 0.0;}
			else {	
				z1 = vertex[2] - del_phi*zeta;
				if (fabs(z1)>zmax){
					del_phi = -(copy_sign(zmax,z1) - vertex[2])/zeta;
				}
			}
		}  
		else { //outside max radius
			return;}
	}		  
	else {	  
		double  x2, y2, gamma, delta, det, del_phi1, del_phi2;
		if (y_c != 0 ) {
			gamma = -x_c/y_c;}
		else	{
			gamma = FLT_MAX;} 
		delta = (rmax*rmax - radius*radius - x_c*x_c - y_c*y_c)/(2*y_c);
		det = 4*gamma*gamma*delta*delta - 4*(1+gamma*gamma)*(delta*delta-radius*radius);
		if (det >= 0) {  /* have intersection */
			det = sqrt(det);
			x1 = (-2*gamma*delta + det)/(2*(1+gamma*gamma));
			x2 = (-2*gamma*delta - det)/(2*(1+gamma*gamma));
			y1 = gamma*x1 + delta;
			y2 = gamma*x2 + delta;
			x1 = x_c + x1;
			x2 = x_c + x2;
			y1 = y_c + y1;
			y2 = y_c + y2;
			/*	Determine which intersection it is... : pos for neg charge. */
			del_phi1 = atan2(-(x1-x_c)/radius,(y1-y_c)/radius) - phi;
			del_phi2 = atan2(-(x2-x_c)/radius,(y2-y_c)/radius) - phi;
			if (charge < 0) { // arc increases wrt time for negative charge
				if (del_phi1<0) {del_phi1 += TWOPI;}
				if (del_phi2<0) {del_phi2 += TWOPI;}}
			else {			// arc decreases wrt time for positive charge
				if (del_phi1>0) {del_phi1 -= TWOPI;}
				if (del_phi2>0) {del_phi2 -= TWOPI;}}
			if ((charge < 0 && del_phi1<del_phi2 ) ||(charge > 0 && del_phi1>del_phi2 ))	{
				del_phi = del_phi1;}
			else {
				x1 = x2;
				y1 = y2; 
				del_phi = del_phi2;}
		}
		else {  /* just put in two loops or z intersection */
			del_phi = - 2.0 * copy_sign(1.0,charge) * TWOPI;
			x1 = vertex[0];
			y1 = vertex[1];
			z1 = time_to_z(angle_to_time(float(del_phi+phi)));
			if (fabs(z1) > zmax) {
				z1 = copy_sign(zmax,z1);
				del_phi = time_to_angle(z_to_time(float(z1-vertex[2])))-phi;}
		}
	}
	if (del_phi != 0.0) {	
		t_1 = angle_to_time(float(del_phi+phi));}
	else {
		t_1 = z_to_time(float(z1-vertex[2]));}   
	DeltaT.setValue(t_1-t_0);
	T_1.setValue(t_1);
	//	Set up any parts of the kit we need
	SoComplexity *complexity = (SoComplexity *) getPart("complexity",TRUE);
	//	complexity->type.setValue(SoComplexity::SCREEN_SPACE);
	complexity->value.setValue(0.7F);
	//	Here we add the calls to generate a polymarker set for the control points
	if (FALSE) { // generate the control points trace points
		SoShapeKit *dPts = (SoShapeKit *) getPart("debugPoints",TRUE);
		dPts->setPart("shape",new SoPointSet);
		SoPointSet *pointSet = (SoPointSet *) dPts->getPart("shape",TRUE);
		SoCoordinate3 *coords = (SoCoordinate3 *) dPts->getPart("coordinate3",TRUE);	  
		dPts->set("drawStyle","pointSize 4");
		dPts->set("drawStyle","style LINES");
		SbVec3f *tracePoints = new SbVec3f[100];
		float t_step = (t_1 - t_0)/100;
		float tt=0;
		for (int ii=0; ii< 100; ii++, tt += t_step){
			float xx, yy;
			time_to_xy(tt).getValue(xx, yy);
			tracePoints[ii].setValue(xx, yy, time_to_z(tt));}
		coords->point.setValues(0, 100, tracePoints);
		pointSet->numPoints = 100;
	}
}                 
void SoIdealTrackKit::updateTrack(){

//  initialise local variables
	particleChar = ParticleChar::getByName(ParticleType.getValue().getString());
	charge = particleChar->getCharge();
	pt = float(SPEEDOLIGHT * 100 * bfield * charge * Radius.getValue());
	double pz = SPEEDOLIGHT * 100 * bfield * charge * Zeta.getValue();
	ptot = (float)sqrt(pt*pt + pz*pz);
    float radius = Radius.getValue();
    float zeta = Zeta.getValue();
    SbVec3f vertex = Vertex.getValue();
    float phi = Phi.getValue();
    float t0 = T0.getValue();
    float t1 = DeltaT.getValue();
	t_0 = T_0.getValue();
	t_1 = T_1.getValue();
    if (t0 < t_0) {t0 = t_0;}
    if (t0 > t_1) {t0 = t_1;}
    if (t1 > t_1) {t1 = t_1;}
    if (t1 < t_0) {t1 = t_0;}
    
    if (fabs(t_1 - t_0) == 0.0 || fabs(t1-t0) == 0.0) {
      setPart("shape",NULL);
      return;}

//  Calculate the start & end points
    float x0, x1, y0, y1, z0, z1;
    float phi_0 = time_to_angle(t0);
    float phi_1 = time_to_angle(t1);  
    float del_phi = phi_1 - phi_0;
//  Until we implement adjustable vector for control points, truncate curve at 2 loops
	if (fabs(del_phi) > 4*M_PI) {
		del_phi = (float)copy_sign(4*M_PI, del_phi);
		phi_1 = del_phi + phi_0;
		t1 = angle_to_time(phi_1);}
    if (del_phi == 0) {
      z0 = time_to_z(t0);
      z1 = time_to_z(t1); }
    else {
      z0 = vertex[2] - (phi_0 - phi)*zeta;
      z1 = vertex[2] - (phi_1 - phi)*zeta; }
      
    double x_c = vertex[0] + radius * sin(phi);
    double y_c = vertex[1] - radius * cos(phi);
    x0 = float(x_c - radius * sin(phi_0));
    y0 = float(y_c + radius * cos(phi_0));
    x1 = float(x_c - radius * sin(phi_1));
    y1 = float(y_c + radius * cos(phi_1));
                     
//  Calculate the control points & knot vector
    SbVec4f control_points[19];
    for (int ii=0; ii < 19; ii++){
      control_points[ii].setValue(0.,0.,0.,0.);} 
    float   knot_vals[22];        //dimension of the control_points + NORDER

                  	
    int ip = 0;        //control point counter
    int ik = 0;        //knot counter
    int ik_count = 0;  //number of knots
    knot_vals[ik] = (float)ik_count; ik++;       /* the first knot value has multiplicity NORDER (=3), put one in now,
                                    and let the normal routine put in the additional NORDER-1.           */
    if (fabs(del_phi) < M_PI/2. ) {           /* if less than pi/2, just use the points. */
      control_points[ip].setValue(x0,y0,z0,1.0);
      knot_vals[ik]   = (float)ik_count; ik++;                  /* put in the knot values for this point: ik_count for NORDER-1 terms */
      knot_vals[ik]   = (float)ik_count; ik++;
      ik_count++;
      ip++;

      control_points[ip].setValue(
        float(x_c - sin(del_phi/2.+phi_0)*radius/fabs(cos(del_phi/2))),
        float(y_c + cos(del_phi/2.+phi_0)*radius/fabs(cos(del_phi/2))),
        (z0+z1)/2,
        1);
      control_points[ip] *= (float)fabs(cos(del_phi/2));
      ip++;}
    else {
/* Must generate some filler points */
/*                                                                              */
/*                 x--------------------------------x                           */
/*                             ||                                               */
/*                             \/                                               */
/*                                                                              */
/*                 x-----o-----x------o----x---o----x                           */
/*                                                                              */
/*                 (one segment becomes three in this example.                  */
/*                  nfill=1 => nfill=3, w 3 x points (normal weight)            */
/*                  and  3 o points to insert                                   */
/*                                                                              */
/*                                                                              */

      int nfill = 1+(int) (fabs(del_phi)/(M_PI/2.));                      /* this should ensure at least one fill division */
      if (nfill == 1) nfill=2;                   /* ensure at least 5 points */
      double del_angle = del_phi/nfill;
      double del_z =  (z1 - z0)/del_phi;              /* change in z wrt angle.       */ 
      double angle = phi_0;     
      for (int ii=0;  ii < nfill ; ii++, angle += del_angle) {
        if (ii==0) {
          control_points[ip].setValue(x0,y0,z0,1.0);} /* put in the first point at exactly the right point. */
        else {
	      radius = Radius.getValue();
          control_points[ip].setValue(
                                  float(x_c - (radius) * sin(angle)),
                                  float(y_c + (radius) * cos(angle)),
                                  float(z0 + del_z*(angle-phi_0)),
                                  1); }
        knot_vals[ik]   = (float)ik_count; ik++;     /* put in the knot values for this point: ik_count for NORDER-1 terms */
        knot_vals[ik]   = (float)ik_count; ik++;
        ik_count++;
	    ip++;

        control_points[ip].setValue(
                       float(x_c - (radius) * sin(angle+(del_angle/2))/
                                               fabs(cos(del_angle/2))),
                       float(y_c + (radius) * cos(angle+(del_angle/2))/
                                                fabs(cos(del_angle/2))),
	              float( z0 + (del_z*(angle+(del_angle/2.)-phi_0))),
	              1);
        control_points[ip] *= (float)fabs(cos(del_angle/2));;
        ip++;} /* end for */
      }  /* end else */
 
/*  the end point */
    control_points[ip].setValue(x1,y1,z1,1.0);
    knot_vals[ik]   = (float)ik_count;  ik++;/* put in the knot values for this point: ik for NORDER terms */
    knot_vals[ik] = (float)ik_count;    ik++;
    knot_vals[ik] = (float)ik_count;    ik++;
    ik_count++;
    ip++;

/*  Define the curve. */
    
    
//  Set the coordinates
	SoCoordinate4 *cpts = (SoCoordinate4 *) getPart("coordinate4",TRUE);
    cpts->point.setValues(0,ip, control_points);
    setPart("coordinate4",cpts);

//  Build the Curve and replace the old one
//  must re-create the NURB each time
    setPart("shape",NULL); //kill off the old one
	SoNurbsCurve *CurveNURB = (SoNurbsCurve *) getPart("shape",TRUE);
    CurveNURB->numControlPoints.setValue(ip);
    CurveNURB->knotVector.setValues(0,ik,knot_vals);

//  Here we add the calls to generate a polymarker set for the control points
    if (FALSE) { // generate the control points trace points
    SoShapeKit *dPts = (SoShapeKit *) getPart("debugPoints",TRUE);
    //SoNode *node = dPts->getPart("shape",TRUE);
    
    SoCoordinate3 *coords = (SoCoordinate3 *) dPts->getPart("coordinate3",FALSE);
    SbVec3f *tracePoints = new SbVec3f[ip];
    for (int ii= 0; ii < ip; ii++){
      control_points[ii].getReal(tracePoints[ii]);
      }
    coords->point.setValues(100, ip, tracePoints);
	delete [] tracePoints;
    SoPointSet *pointSet = (SoPointSet *) dPts->getPart("shape",FALSE);
    pointSet->numPoints = ip+100;
	}   
}
void SoIdealTrackKit::generateAlternateRep(SoAction*)
{
	alternateRep.setValue(topSeparator.getValue());
}
void SoIdealTrackKit::clearAlternateRep()
{
	alternateRep.setValue(NULL);
}
