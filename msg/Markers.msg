# List of Markers detected by a sensor
#

Header header            # timestamp in the header is the acquisition time
uint32 num_markers       # Number of detected markers
int32[] id               # Detected marker id (-1 if id not detected)
float64[] bearing        # Angle to the marker
float64[] range          # range to the marker
