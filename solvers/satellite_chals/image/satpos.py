from skyfield.api import *
import logging, argparse
from datetime import datetime, timezone, timedelta

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

def configure_logging():

    # Define ANSI escape sequences for colors
    COLORS = {
        'CRITICAL': '\033[91m',  # Red
        'ERROR': '\033[91m',     # Red
        'WARNING': '\033[93m',   # Yellow
        'INFO': '\033[92m',      # Green
        'DEBUG': '\033[94m',     # Blue
        'RESET': '\033[0m'       # Reset to default color
    }

    # Set up the logging configuration with color formatting
    class ColorFormatter(logging.Formatter):
        def format(self, record):
            levelname = record.levelname
            message = super().format(record)
            color = COLORS.get(levelname, COLORS['RESET'])
            pre = f"{color}{levelname}{COLORS['RESET']}:"
            return f"{pre:19}{message}"

    # Create a logger and set its level
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)  # Set the desired logging level for asyncio

    # Add color formatter to the loggers
    formatter = ColorFormatter()
    handler = logging.StreamHandler()
    handler.setFormatter(formatter)

    logger.addHandler(handler)

class Sat():
    def __init__(self, id):
        url = 'http://celestrak.org/NORAD/elements/gp.php?GROUP=active&FORMAT=tle'
        self.ts = load.timescale()
        satellites = load.tle_file(url)
        
        by_name = {sat.name: sat for sat in satellites}
        self.sat: EarthSatellite = by_name[id]
        logger.info(f"Loaded {id}")
    
    def at(self, dt: datetime):
        dt = dt.replace(tzinfo=timezone.utc)
        t = self.ts.from_datetime(dt)
        return self.sat.at(t).position.km # ECI position

    # Time of closest approach
    def toca(self, dt0: datetime, dt1: datetime, lat, long, alt):
        # LLA in degrees and meters
        t0 = self.ts.from_datetime(dt0.replace(tzinfo=timezone.utc))
        t1 = self.ts.from_datetime(dt1.replace(tzinfo=timezone.utc))
        sat = self.sat
        target = wgs84.latlon(lat, long, alt)
        
        duration_seconds = int((t1-t0)*24*60*60)
        t = t0
        min_dist = 2**64
        t_closest = t0
        for _ in range (duration_seconds):   
            r = sat - target
            topocentric = r.at(t)
            el, az, distance = topocentric.altaz()
            distance = distance.km
            if distance < min_dist:
                min_dist = distance
                t_closest = t
                logger.debug(f"Updated min_dist: {t_closest.utc_strftime()} {min_dist}")
            t = t + 1/(24*60*60)
        logger.info(f"TOCA: {t_closest.utc_strftime()} {min_dist}")
        return t_closest, min_dist
    
    def LLA(self, dt):
        t = self.ts.from_datetime(dt.replace(tzinfo=timezone.utc))
        #lat, long, alt = wgs84.geographic_position_of(pos)
        sp = wgs84.subpoint_of(self.sat.at(t))
        lat = sp.latitude.degrees
        lon = sp.longitude.degrees
        alt = sp.elevation.km
        logger.debug(f"LLA at {t.utc_strftime()}: {lat}, {lon}, {alt}")
        return lat, lon, alt
