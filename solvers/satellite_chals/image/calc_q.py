import skyfield.api as sf
from datetime import datetime
import numpy as np
import csv
import satpos
from collections import OrderedDict
ts = sf.load.timescale()
from scipy.spatial.transform import Rotation as R


def quat_to_dcm( qAxis , qScalar ):
    x = qAxis[0]
    y = qAxis[1]
    z = qAxis[2]
    w  = qScalar
    q = [x,y,z,w]
    rot_I2B = R.from_quat( q )
    # dcm for rotations of column vectors
    dcm_I2B = rot_I2B.as_matrix().transpose()
    return dcm_I2B
def quatFromDcm( C ):
    q4 = np.sqrt( (1 + C[0][0] + C[1][1] + C[2][2])/4)
    q =  np.array( [ C[1][2]-C[2][1] , C[2][0] - C[0][2] , C[0][1] - C[1][0] , 4*q4*q4 ] )
    q = q/(4*q4)
    return q

time_format = "%Y-%m-%d-%H:%M:%S.%f"
trunc_format = "%Y-%m-%d %H:%M:%S"
class QuatCalc:

    def __init__( self , satid , q_plan_file):
        self.plan  = OrderedDict()
        self.satid = satid
        self.sat = satpos.Sat( satid )
        with open( q_plan_file, newline='\n') as csvfile:
            planReader = csv.reader(csvfile, delimiter=',')
            next( planReader )
            for row in planReader:
                timeStr = row[0]
                qx = float( row[1])
                qy = float( row[2])
                qz = float( row[3])
                qS = float( row[4])
                qAxis = np.array( [qx,qy,qz])
                print(f"{timeStr} {qAxis} {qS}")
                dto = datetime.strptime( timeStr , time_format )
                timeSave  = dto.strftime( trunc_format )
                self.plan[ timeSave ] = (qAxis, qS )
                

    def q_calc( self,  latDeg , longDeg  , hMeter ):
        t0 = datetime.strptime( list( self.plan.keys())[0] , trunc_format )
        tf = datetime.strptime( list( self.plan.keys())[-1], trunc_format )
        
        sfTime, dist = self.sat.toca( t0 , tf , latDeg , longDeg, hMeter )
        dto = sfTime.utc_datetime()
        timeStr = dto.strftime( trunc_format )
        posEci = self.sat.at( dto )
        
        target = sf.wgs84.latlon(latDeg, longDeg,  hMeter )
        targetEci = target.at( sfTime ).position.km
        plan = self.plan[timeStr]
        qAxis = plan[0]
        qS = plan[1]
        dcm_I2B = quat_to_dcm( qAxis , qS)
        # Calculate an axis of rotation that is normal to the body z axis
        
        # Given the satellite will point z nadir in the body frame calculate nadir in eci
        nadir = -posEci
        nadirUnut = nadir / np.linalg.norm( nadir )
        # Calculate line of sight from the satellite to the target in eci as well
        targetLos = -posEci + targetEci
        losUnit = targetLos / np.linalg.norm( targetLos )
        # Perform a coordinate transformation into the body frame
        losBodyUnit = np.matmul( dcm_I2B , losUnit )
        nadirBodyUnit = np.matmul( dcm_I2B , nadirUnut )
        # Use the dot product to calculate the total angle we must rotate
        dp = np.dot( losBodyUnit , nadirBodyUnit )
        dp = np.clip( dp , -1.0 , 1.0 )
        angle = np.arccos(  dp ) #rad
        angleDeg = angle * 180.0 / np.pi 
        # Use the cross product to determine the axis which we must rotate about in the body frame
        rotAxis = np.cross( nadirBodyUnit , losBodyUnit )
        rotAxis = rotAxis / np.linalg.norm( rotAxis )
        # Use modified rodrigues parameters to get a rotation matrix
        rot2 = R.from_mrp( np.tan( angle/4 ) * rotAxis )
        #rot2 =  R.from_rotvec( angle * rotAxis  )
        dcm2 = rot2.as_matrix()
        dcm2 = np.transpose( dcm2 )     # dcm for rotations of column vectors
        
        print( f"second rotation matrix {dcm2}")
        # Use successive rotations to go from I->Bplan->Bfinal
        full_dcm = np.matmul( dcm2 , dcm_I2B )
        finalQuat = quatFromDcm( full_dcm )
        print( f"Ecef position {1 }")
        print( f"Original quaternion {plan}")
        print( f"POCA at {dto.strftime( time_format)}")
        print(f"Rotation axis (Body): {rotAxis}")
        print(f"Rotation angle: {angleDeg }")
        print( f"Quaternion {finalQuat}")
        # Calculating the point of intersection of the LOS vector with elliptical earth should give
        # a position within 1-2 km of the target 
        print("DONE")
        
if __name__ == "__main__":

    d = QuatCalc( "MOONLIGHTER", "example_plan.csv")

    # Winnepeg 49.900296,-97.141113
    d.q_calc( 49.900296, -97.141113 , 0.0 )