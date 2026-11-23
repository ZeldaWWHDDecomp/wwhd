#!/usr/bin/env python3
"""Self-test of compare60.py (true60 T-03) on synthetic run folders in the trace formats of the port's test aids.

A: a 30 Hz run (one logic step per frame, dt 1). B: the same scenario at N = 2 (and 4) steps per frame (dt 1/N, full on
the last step of a frame), Link converted (an execute every step), an enemy unconverted (executes on full steps only).
Link accelerates with the exact arc, so his full-step states equal A's (up to a small float noise).
Positive: B with steps.txt (N 2 and 4), B without steps.txt (K from the dt column), a procedure change one frame late, a
sound one frame late, sound starts and a process before the origin that A does not have (the state load is no frame), a
looping animation 0.3 frame late across its loop wrap (time slack). Time slack: a position 5 units off at one frame and Link 0.3 frame late fail link-motion only
(exit 3, equivalent under the time slack); 40 units off and 1.5 frames late fail both forms. Negative (must fail exactly
the named gate): procedure change 3 frames late (link-procs), an extra sound (sound), a new process profile (procs), a save byte (save; passes
with --save-mask; the counter-edge rule (8c): a listed counter one count off at a converted state change passes, two counts, an
unlisted field or no converted change near the edge fail), the animation-phase lead rule (6d(c)) (pgrab5's animation-change frame passes with the
change frame's pose-gap term, plus a 1.0 speed error fails, no change: no term, a differing base foot: no term, reported;
addendum: pfol1's base-foot switch frame passes with B's foot's pose gaps + A's foot-choice chord gap, a 3.0 error fails,
equal chord feet get the pose term only, a switch in one run with differing chord feet gets no term, reported),
NaN in the trace (health), B at dt 1 (mode), an actor-dump word out of its spec tolerance (admp); the cloth-simulation rule (6k) (a cloth
with sf3's per-vertex divergence and one 1.2 x max sample pass; doubled flutter speed, a 1.5 x max sample, a stuck cloth and a
too-short window fail; 6k bound: an independent flutter sample of gy13n4o's form passes, the
same sail offset by 1 sd for the whole window and a stuck one fail); the speed-ramp position-gap rule (6c) camera
(yaw episodes): the pair's measured manualCamera yaw passes cam-tslack with the term only, plus a real 5 deg error fails,
no stick input adds nothing, the same yaw in a follow style gets no term; the speed-ramp position-gap rule (6c) camera, centre chase: pkk1d_4's
measured centres while locked on pass with the g |v| term only, a real 5-unit error at frame 54 fails, the same centres
following Link get no term.
the strict RNG-pair rule (8d) (actor-plaus, --plaus-spec): a converted enemy's other RNG choices after the RNG difference pass, a stuck
state, a NaN, health going wrong and an attack-rate blowup fail; the strict RNG-pair rule (8d) addendum (--strict-until / --cause): an
enemy that pushes Link, both split at the cut, passes with a cause; without the cut the strict gates fail; a cut without a
valid cause, or a cause without a cut, fails strict-cut; a real divergence before the cut (Link or the enemy) fails.
Addenda 2 / 3: a cause on the enemy's side (side=enemy rule=3) passes, one naming no accepted
rule for its side fails strict-cut; after the cut Link's health in the save may differ within A's range (save-life), a
save that differs in anything else fails save, health outside A's range or a health difference before the cut fails.
Addendum 4: a Link-side the first-step input rule (12a) cause (process order, unconverted Link) passes
only with Link unconverted in B; with Link converted it fails strict-cut.
the speed-ramp position-gap rule (6c) for an enemy's own motion: the measured ChuChu knock-back flights pass
the admp gate with the spec's 6c term; a gap 1.5 x the term, a gap without a ramp across the steps, or a differing
discrete word fail; side=enemy rule=6c is a valid cut cause.
the step-resolution threshold rule (6l) addendum (f): `rule=6l hit=life` (a hit sampled at step resolution)
passes with the cut on B's hit one frame before A's (or on A's frame), on either side; two frames early, one frame late,
a hit A does not have near it, or side=enemy rule=6l without hit= fail strict-cut (side=link rule=6l without hit= stays
Link-side 6l cause).
Folders go to a temporary directory under $T60_TEST_TMP (default: the scratch RAM disk) and are removed.
"""
import collections
import math
import os
import random
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import math

HERE = os.path.dirname(os.path.abspath(__file__))
FRAMES = 90
ARRIVE_Z = 1630.0  # the early ship-decision rule (6i(iii)) test: the DEMO_MOVE target z (A stops at frame 60)
# pgrab5 Link frames 115-122: (speedF, mNormalSpeed, anim frame, rate, base foot, toe0 x z, toe1 x z);
# A one record per frame, B its two steps per frame (the animation-phase lead rule (6d(c)): the animation change at 120)
PGRAB5_A = ((16.3075, 16.533, 28.179, 2.218, 0, -10.1874, 24.5068, 10.1838, -16.5523),
            (16.3107, 16.533, 30.396, 2.218, 0, -10.1948, 37.1436, 10.0401, -17.8628),
            (11.0645, 14.033, 0.614, 1.776, 0, -10.1113, 42.6741, 10.0174, -16.5409),
            (8.5058, 11.533, 2.39, 1.335, 0, -9.722, 32.8309, 9.4196, -14.3308),
            (7.5683, 9.033, 3.725, 0.894, 0, -10.8115, 23.9256, 9.4193, -13.9624),
            (4.2869, 6.533, 8.539, 1.391, 0, -10.5178, 19.6488, 9.9856, -14.8091),
            (4.7479, 4.033, 9.93, 1.28, 0, -11.946, 15.1208, 10.6695, -10.3639),
            (5.0104, 1.613, 11.21, 1.172, 0, -13.2297, 10.2776, 11.7722, -4.1276))
PGRAB5_B = (((16.2873, 16.533, 27.42, 2.218, 0, -10.1848, 26.6172, 10.1539, -16.9331), (16.3056, 16.533, 28.529, 2.218, 0, -10.1953, 33.069, 10.0588, -17.814)),
            ((16.3074, 16.533, 29.637, 2.218, 0, -10.1845, 38.9364, 10.0378, -17.7891), (16.2881, 16.533, 30.746, 2.218, 0, -10.1355, 41.9539, 10.0469, -17.2589)),
            ((13.4141, 15.283, 31.855, 1.997, 0, -10.0819, 42.7918, 10.0041, -16.0663), (9.7193, 14.033, 0.853, 1.776, 0, -9.6394, 37.4071, 9.6556, -14.782)),
            ((9.3881, 12.783, 1.742, 1.556, 0, -9.8458, 31.9807, 9.415, -14.1138), (7.9392, 11.533, 2.519, 1.335, 0, -10.4384, 27.9422, 9.3365, -13.8857)),
            ((7.3799, 10.283, 3.187, 1.115, 0, -10.8658, 23.0699, 9.4122, -13.8495), (7.2145, 9.033, 3.744, 0.894, 0, -10.853, 20.1466, 9.65, -14.0871)),
            ((5.8467, 7.783, 7.748, 1.447, 0, -10.5183, 19.5868, 9.9854, -14.7987), (1.1542, 6.533, 8.471, 1.391, 0, -10.9839, 18.0408, 10.2837, -13.5377)),
            ((3.2292, 5.283, 9.167, 1.335, 0, -11.9532, 15.2065, 10.6688, -10.3827), (5.9894, 4.033, 9.834, 1.28, 0, -12.6912, 12.6395, 11.168, -7.2318)),
            ((5.342, 2.783, 10.474, 1.224, 0, -13.2444, 10.3551, 11.7716, -4.1452), (4.6985, 1.76, 11.086, 1.178, 0, -13.6611, 8.3683, 12.4724, -1.1399)))
# pgrab6 (port m3598 column, trace column 32): m3598 (the foot-travel weight r) of the same records
PGRAB5_RA = (0.055, 0.055, 0.349118, 0.643235, 0.937353, 1.0, 1.0, 1.0)
PGRAB5_RB = ((0.055, 0.055), (0.055, 0.055), (0.202059, 0.349118), (0.496176, 0.643235), (0.790294, 0.937353), (1.0, 1.0),
             (1.0, 1.0), (1.0, 1.0))
# pfol1 Link frames 102-109: (speedF, mNormalSpeed, anim frame, rate, base foot,
# toe0 x z, toe1 x z, m3598); A one record per frame, B its two steps per frame (the animation-phase lead rule (6d(c)) addendum: the walk stop
# at the stick release; A switches its base foot 0 -> 1 at 107, B on 107's second step: chord feet A 1, B 0)
PFOL1_A = ((1.7325, 7.650, 12.997, 1.431, 0, -10.8752, 16.8582, 10.6098, -8.5180, 1.0),
           (1.6577, 7.650, 14.428, 1.431, 0, -10.8499, 14.8501, 10.5445, -7.5196, 1.0),
           (1.5523, 7.650, 15.859, 1.431, 0, -10.8147, 12.9700, 10.4740, -6.1345, 1.0),
           (1.6690, 7.650, 17.290, 1.431, 0, -10.8094, 10.9485, 10.5090, -4.0175, 1.0),
           (1.7306, 5.150, 18.721, 1.323, 0, -10.7653, 8.8529, 10.4583, -0.9615, 1.0),
           (4.2778, 2.650, 20.044, 1.215, 1, -11.7546, 6.5040, 11.5831, 4.0962, 1.0),
           (2.3731, 0.850, 21.259, 1.137, 1, -12.8518, 5.1965, 13.0720, 6.5549, 1.0),
           (0.0000, 0.000, 22.395, 1.100, 1, -13.8848, 4.9686, 14.0188, 6.0572, 0.0))
PFOL1_B = (((1.8086, 7.650, 12.335, 1.431, 0, -10.8771, 16.5729, 10.6247, -8.4647, 1.0), (1.7578, 7.650, 13.050, 1.431, 0, -10.8482, 15.6621, 10.5705, -8.0151, 1.0)),
           ((1.5045, 7.650, 13.766, 1.431, 0, -10.8372, 14.5694, 10.5123, -7.4643, 1.0), (1.8038, 7.650, 14.481, 1.431, 0, -10.7952, 13.6297, 10.4841, -6.8861, 1.0)),
           ((1.5530, 7.650, 15.197, 1.431, 0, -10.7706, 12.6587, 10.4733, -6.0096, 1.0), (1.6037, 7.650, 15.912, 1.431, 0, -10.7764, 11.6879, 10.4664, -5.0644, 1.0)),
           ((1.6031, 7.650, 16.628, 1.431, 0, -10.7795, 10.6022, 10.4919, -3.9429, 1.0), (1.7928, 7.650, 17.343, 1.431, 0, -10.7812, 9.7106, 10.4591, -2.4912, 1.0)),
           ((1.4722, 6.400, 18.059, 1.377, 0, -10.7774, 8.5669, 10.4825, -0.8431, 1.0), (1.8885, 5.150, 18.747, 1.323, 0, -11.2446, 7.3215, 10.9462, 1.9615, 1.0)),
           ((2.1963, 3.900, 19.409, 1.269, 0, -11.7161, 6.2520, 11.5822, 4.1358, 1.0), (1.9284, 2.650, 20.043, 1.215, 1, -12.2391, 5.5119, 12.3149, 5.7628, 1.0)),
           ((2.9465, 1.676, 20.651, 1.173, 1, -12.8351, 5.0450, 13.0568, 6.5235, 1.0), (1.6875, 0.776, 21.237, 1.134, 1, -13.3609, 4.8628, 13.6151, 6.5063, 1.0)),
           ((0.0000, 0.000, 21.804, 1.100, 1, -13.9023, 4.8770, 14.0392, 5.9311, 0.0), (0.0000, 0.000, 22.354, 1.100, 1, -14.2135, 5.1726, 14.3252, 6.0959, 0.0)))
# (manualCamera MM01, style 57 from frame 61) frames 56-180: frame-end view yaw
# (deg, atan2 of eye - centre in x / z) and horizontal eye-centre radius, A (30 Hz) and B (N = 2): the stick (1, 0) released
# at frame 90 (B ends 3.7 deg past A), the pitch (0, 1), the turn (-0.6, -1) at 135-151 (B ends 3.79 deg past A)
PMAN2_A = ((104.635, 200.0), (104.635, 200.0), (104.635, 200.0), (104.635, 200.0), (104.635, 200.0), (104.635, 200.0), (104.126, 200.0), (102.993, 200.0), (101.153, 200.0), (98.582, 200.0), (95.280, 200.0), (91.245, 200.0), (86.471, 200.0), (80.957, 200.0), (74.716, 200.0), (67.735, 200.0), (60.495, 200.0), (53.177, 200.0), (45.836, 200.0), (38.482, 200.0), (31.118, 200.0), (23.758, 200.0), (16.395, 200.0), (9.034, 200.0), (1.679, 200.0), (-5.676, 200.0), (-13.031, 200.0), (-20.387, 200.0), (-27.738, 200.0), (-35.092, 200.0), (-42.454, 200.0), (-49.810, 200.0), (-57.159, 200.0), (-64.514, 200.0), (-71.875, 200.0), (-78.015, 200.0), (-82.531, 200.0), (-85.277, 200.0), (-86.211, 200.0), (-86.524, 200.0), (-86.632, 200.0), (-86.668, 200.0), (-86.686, 200.0), (-86.686, 200.0), (-86.686, 200.0), (-86.686, 200.0), (-86.686, 200.0), (-86.686, 200.0), (-86.686, 200.0), (-86.686, 200.0), (-86.690, 186.7), (-86.687, 168.7), (-86.690, 161.8), (-86.686, 156.7), (-86.693, 152.8), (-86.692, 149.5), (-86.687, 146.5), (-86.684, 143.7), (-86.682, 140.9), (-86.691, 138.1), (-86.684, 135.1), (-86.685, 131.9), (-86.683, 131.2), (-86.686, 130.8), (-86.692, 130.5), (-86.687, 130.3), (-86.684, 130.2), (-86.681, 130.1), (-86.694, 130.0), (-86.693, 130.0), (-86.692, 130.0), (-86.691, 129.9), (-86.691, 129.9), (-86.691, 129.9), (-86.691, 129.9), (-86.691, 129.9), (-86.691, 129.9), (-86.691, 129.9), (-86.691, 129.9), (-86.691, 129.9), (-87.177, 140.2), (-85.970, 151.2), (-84.047, 163.9), (-81.396, 177.8), (-78.369, 193.1), (-75.210, 210.6), (-72.012, 230.8), (-68.813, 253.9), (-65.645, 270.4), (-62.432, 291.2), (-59.221, 315.7), (-56.078, 344.6), (-52.866, 373.5), (-49.663, 408.4), (-46.334, 437.1), (-44.400, 443.1), (-43.744, 445.1), (-43.521, 445.7), (-43.447, 446.0), (-43.417, 446.1), (-43.412, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1), (-43.406, 446.1))
PMAN2_B = ((104.635, 200.0), (104.635, 200.0), (104.635, 200.0), (104.635, 200.0), (104.635, 200.0), (104.533, 200.0), (103.987, 200.0), (102.801, 200.0), (100.907, 200.0), (98.292, 200.0), (94.930, 200.0), (90.842, 200.0), (86.013, 200.0), (80.458, 200.0), (74.159, 200.0), (67.201, 200.0), (59.980, 200.0), (52.670, 200.0), (45.323, 200.0), (37.966, 200.0), (30.608, 200.0), (23.244, 200.0), (15.886, 200.0), (8.528, 200.0), (1.168, 200.0), (-6.185, 200.0), (-13.541, 200.0), (-20.899, 200.0), (-28.251, 200.0), (-35.606, 200.0), (-42.961, 200.0), (-50.316, 200.0), (-57.668, 200.0), (-65.029, 200.0), (-72.383, 200.0), (-79.056, 200.0), (-84.279, 200.0), (-87.788, 200.0), (-89.508, 200.0), (-90.090, 200.0), (-90.296, 200.0), (-90.358, 200.0), (-90.385, 200.0), (-90.394, 200.0), (-90.394, 200.0), (-90.394, 200.0), (-90.394, 200.0), (-90.394, 200.0), (-90.394, 200.0), (-90.394, 200.0), (-90.388, 189.2), (-90.385, 172.1), (-90.393, 164.1), (-90.385, 158.4), (-90.396, 154.0), (-90.393, 150.4), (-90.389, 147.3), (-90.385, 144.4), (-90.392, 141.6), (-90.388, 138.7), (-90.396, 135.7), (-90.392, 132.5), (-90.394, 131.7), (-90.396, 131.1), (-90.384, 130.7), (-90.384, 130.5), (-90.385, 130.3), (-90.385, 130.2), (-90.385, 130.1), (-90.386, 130.0), (-90.386, 130.0), (-90.386, 129.9), (-90.386, 129.9), (-90.386, 129.9), (-90.386, 129.9), (-90.386, 129.9), (-90.386, 129.9), (-90.386, 129.9), (-90.386, 129.9), (-90.386, 129.9), (-90.127, 141.1), (-90.187, 153.4), (-88.561, 166.7), (-86.226, 181.3), (-83.326, 197.4), (-80.213, 215.5), (-77.036, 236.4), (-73.855, 260.1), (-70.711, 276.5), (-67.495, 297.3), (-64.289, 321.6), (-61.077, 350.5), (-57.932, 380.4), (-54.498, 415.3), (-51.348, 433.8), (-48.890, 442.0), (-47.768, 444.7), (-47.390, 445.6), (-47.258, 445.9), (-47.213, 446.1), (-47.199, 446.1), (-47.196, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1), (-47.197, 446.1))
CAM_LATE_ERR = 7.5  # the eye's bounded tolerance at frame 85 of this route is 6.46 (2 + Link's offset 0 + camera drift and v/6 4.46); 6 passes, 7.5 fails

# the animation-phase lead rule (6d) addendum (N 4, measured: frames 254-320): Link's wait
# animation (proc 4, loop 60) frame / rate, A per frame and B per step; the rate ramp 265-285 and the drop at 286
PRUN4A_A = ((5.289, 1.100), (6.389, 1.100), (7.489, 1.100), (8.589, 1.100), (9.689, 1.100), (10.789, 1.100), (11.889, 1.100), (12.989, 1.100), (14.089, 1.100), (15.189, 1.100), (16.289, 1.100), (17.389, 2.248), (19.637, 2.233), (21.870, 2.223), (24.093, 2.210), (26.303, 2.208), (28.511, 2.208), (30.719, 2.208), (32.927, 2.208), (35.136, 2.208), (37.344, 1.100), (38.444, 2.176), (40.620, 1.100), (41.720, 2.314), (44.033, 2.413), (46.446, 2.459), (48.905, 2.478), (51.384, 2.472), (53.855, 2.455), (56.310, 2.430), (58.740, 2.402), (1.142, 2.375), (3.517, 1.100), (4.617, 1.100), (5.717, 1.100), (6.817, 1.100), (7.917, 1.100), (9.017, 1.100), (10.117, 1.100), (11.217, 1.100), (12.317, 1.100), (13.417, 1.100), (14.517, 1.100), (15.617, 1.100), (16.717, 1.100), (17.817, 1.100), (18.917, 1.100), (20.017, 1.100), (21.117, 1.100), (22.217, 1.100), (23.317, 1.100), (24.417, 1.100), (25.517, 1.100), (26.617, 1.100), (27.717, 1.100), (28.817, 1.100), (29.917, 1.100), (31.017, 1.100), (32.117, 1.100), (33.217, 1.100), (34.317, 1.100), (35.417, 1.100), (36.517, 1.100), (37.617, 1.100), (38.717, 1.100), (39.817, 1.100), (40.917, 1.100))
PRUN4A_B = (((4.190, 1.100), (4.465, 1.100), (4.740, 1.100), (5.015, 1.100)), ((5.290, 1.100), (5.565, 1.100), (5.840, 1.100), (6.115, 1.100)), ((6.390, 1.100), (6.665, 1.100), (6.940, 1.100), (7.215, 1.100)), ((7.490, 1.100), (7.765, 1.100), (8.040, 1.100), (8.315, 1.100)), ((8.590, 1.100), (8.865, 1.100), (9.140, 1.100), (9.415, 1.100)), ((9.690, 1.100), (9.965, 1.100), (10.240, 1.100), (10.515, 1.100)), ((10.790, 1.100), (11.065, 1.100), (11.340, 1.100), (11.615, 1.100)), ((11.890, 1.100), (12.165, 1.100), (12.440, 1.100), (12.715, 1.100)), ((12.990, 1.100), (13.265, 1.100), (13.540, 1.100), (13.815, 1.100)), ((14.090, 1.100), (14.365, 1.100), (14.640, 1.100), (14.915, 1.100)), ((15.190, 1.100), (15.465, 1.100), (15.740, 1.100), (16.015, 1.100)), ((16.290, 2.183), (16.836, 2.208), (17.388, 2.231), (17.946, 2.248)), ((18.508, 2.178), (19.053, 2.200), (19.602, 2.221), (20.158, 2.235)), ((20.716, 2.174), (21.260, 2.193), (21.808, 2.210), (22.361, 2.223)), ((22.917, 2.172), (23.460, 2.187), (24.006, 2.202), (24.557, 2.210)), ((25.109, 2.170), (25.652, 2.185), (26.198, 2.200), (26.748, 2.208)), ((27.300, 2.170), (27.842, 2.185), (28.389, 2.200), (28.939, 2.208)), ((29.491, 2.170), (30.033, 2.185), (30.579, 2.200), (31.129, 2.208)), ((31.681, 2.170), (32.224, 2.185), (32.770, 2.200), (33.320, 2.208)), ((33.872, 2.170), (34.414, 2.185), (34.960, 2.200), (35.510, 2.208)), ((36.062, 2.170), (36.605, 2.185), (37.151, 2.195), (37.700, 2.195)), ((38.249, 1.100), (38.524, 1.100), (38.799, 1.100), (39.074, 1.100)), ((39.349, 2.170), (39.891, 2.178), (40.436, 2.178), (40.981, 2.178)), ((41.525, 1.100), (41.800, 1.100), (42.075, 1.100), (42.350, 1.100)), ((42.625, 2.200), (43.175, 2.242), (43.736, 2.282), (44.306, 2.318)), ((44.885, 2.181), (45.431, 2.204), (45.982, 2.227), (46.538, 2.242)), ((47.099, 2.212), (47.652, 2.267), (48.219, 2.318), (48.798, 2.362)), ((49.389, 2.229), (49.946, 2.299), (50.521, 2.366), (51.112, 2.425)), ((51.718, 2.238), (52.278, 2.316), (52.857, 2.390), (53.454, 2.453)), ((54.067, 2.238), (54.627, 2.316), (55.206, 2.390), (55.803, 2.455)), ((56.417, 2.235), (56.976, 2.311), (57.554, 2.383), (58.149, 2.447)), ((58.761, 2.229), (59.318, 2.299), (59.893, 2.366), (0.484, 2.425)), ((1.091, 1.100), (1.366, 1.100), (1.641, 1.100), (1.916, 1.100)), ((2.191, 1.100), (2.466, 1.100), (2.741, 1.100), (3.016, 1.100)), ((3.291, 1.100), (3.566, 1.100), (3.841, 1.100), (4.116, 1.100)), ((4.391, 1.100), (4.666, 1.100), (4.941, 1.100), (5.216, 1.100)), ((5.491, 1.100), (5.766, 1.100), (6.041, 1.100), (6.316, 1.100)), ((6.591, 1.100), (6.866, 1.100), (7.141, 1.100), (7.416, 1.100)), ((7.691, 1.100), (7.966, 1.100), (8.241, 1.100), (8.516, 1.100)), ((8.791, 1.100), (9.066, 1.100), (9.341, 1.100), (9.616, 1.100)), ((9.891, 1.100), (10.166, 1.100), (10.441, 1.100), (10.716, 1.100)), ((10.991, 1.100), (11.266, 1.100), (11.541, 1.100), (11.816, 1.100)), ((12.091, 1.100), (12.366, 1.100), (12.641, 1.100), (12.916, 1.100)), ((13.191, 1.100), (13.466, 1.100), (13.741, 1.100), (14.016, 1.100)), ((14.291, 1.100), (14.566, 1.100), (14.841, 1.100), (15.116, 1.100)), ((15.391, 1.100), (15.666, 1.100), (15.941, 1.100), (16.216, 1.100)), ((16.491, 1.100), (16.766, 1.100), (17.041, 1.100), (17.316, 1.100)), ((17.591, 1.100), (17.866, 1.100), (18.141, 1.100), (18.416, 1.100)), ((18.691, 1.100), (18.966, 1.100), (19.241, 1.100), (19.516, 1.100)), ((19.791, 1.100), (20.066, 1.100), (20.341, 1.100), (20.616, 1.100)), ((20.891, 1.100), (21.166, 1.100), (21.441, 1.100), (21.716, 1.100)), ((21.991, 1.100), (22.266, 1.100), (22.541, 1.100), (22.816, 1.100)), ((23.091, 1.100), (23.366, 1.100), (23.641, 1.100), (23.916, 1.100)), ((24.191, 1.100), (24.466, 1.100), (24.741, 1.100), (25.016, 1.100)), ((25.291, 1.100), (25.566, 1.100), (25.841, 1.100), (26.116, 1.100)), ((26.391, 1.100), (26.666, 1.100), (26.941, 1.100), (27.216, 1.100)), ((27.491, 1.100), (27.766, 1.100), (28.041, 1.100), (28.316, 1.100)), ((28.591, 1.100), (28.866, 1.100), (29.141, 1.100), (29.416, 1.100)), ((29.691, 1.100), (29.966, 1.100), (30.241, 1.100), (30.516, 1.100)), ((30.791, 1.100), (31.066, 1.100), (31.341, 1.100), (31.616, 1.100)), ((31.891, 1.100), (32.166, 1.100), (32.441, 1.100), (32.716, 1.100)), ((32.991, 1.100), (33.266, 1.100), (33.541, 1.100), (33.816, 1.100)), ((34.091, 1.100), (34.366, 1.100), (34.641, 1.100), (34.916, 1.100)), ((35.191, 1.100), (35.466, 1.100), (35.741, 1.100), (36.016, 1.100)), ((36.291, 1.100), (36.566, 1.100), (36.841, 1.100), (37.116, 1.100)), ((37.391, 1.100), (37.666, 1.100), (37.941, 1.100), (38.216, 1.100)), ((38.491, 1.100), (38.766, 1.100), (39.041, 1.100), (39.316, 1.100)))


# the per-step camera-collision rule (6b) episode: cy7 (crawl pair, N 2) frames 196-234, the camera's
# frame-end eye xyz and centre xyz (A, B) relative to A's frame-196 centre: B's in-between-step wall hit at 204.5 leaves
# the cushioned direction below the view, B's wall release (eye radius jump) at 207, A's at 227; the eyes converge at 228
CY7_A = (
    (430.37, 114.46, 119.28, 0.00, 0.00, 0.00),
    (429.52, 114.02, 118.03, -0.68, 0.00, 0.00),
    (428.27, 113.78, 116.63, -2.47, 0.00, 0.00),
    (425.98, 113.63, 114.02, -5.95, 0.00, 0.00),
    (422.44, 113.48, 110.79, -10.85, 0.00, 0.00),
    (418.15, 113.26, 106.76, -16.55, 0.00, 0.00),
    (341.16, 91.20, 86.40, -22.31, 0.00, 0.00),
    (282.61, 74.65, 71.13, -27.34, 0.00, 0.00),
    (238.17, 62.24, 59.68, -30.97, 0.00, 0.00),
    (107.70, 24.50, 25.32, -31.30, 3.64, 0.00),
    (101.07, 24.50, 22.65, -31.81, 6.91, 0.00),
    (97.50, 24.50, 17.24, -32.97, 9.82, 0.00),
    (96.21, 24.50, 12.99, -35.18, 12.36, 0.00),
    (97.12, 24.50, 9.59, -38.73, 14.55, 0.00),
    (100.87, 24.50, 6.80, -43.67, 16.36, 0.00),
    (110.59, 24.50, 4.48, -48.17, 17.82, 0.00),
    (182.29, 39.80, 3.30, -52.18, 18.91, 0.00),
    (162.27, 35.35, 1.02, -55.65, 19.64, 0.00),
    (144.96, 31.74, 0.00, -58.49, 20.00, 0.00),
    (132.73, 28.85, 0.00, -59.10, 20.00, 0.00),
    (122.62, 26.74, 0.00, -59.88, 20.00, 0.00),
    (114.35, 25.17, 0.00, -60.67, 20.00, 0.00),
    (107.65, 24.00, 0.00, -61.39, 20.00, 0.00),
    (102.24, 23.12, 0.00, -62.00, 20.00, 0.00),
    (97.90, 22.45, 0.00, -62.49, 20.00, 0.00),
    (94.47, 21.93, 0.00, -62.85, 20.00, 0.00),
    (91.78, 21.53, 0.00, -63.08, 20.00, 0.00),
    (89.71, 21.22, 0.00, -63.17, 20.00, 0.00),
    (88.10, 20.97, 0.00, -63.21, 20.00, 0.00),
    (86.83, 20.78, 0.00, -63.22, 20.00, 0.00),
    (85.81, 20.63, 0.00, -63.23, 20.00, 0.00),
    (85.00, 20.51, 0.00, -63.23, 20.00, 0.00),
    (81.77, 20.00, 0.00, -63.23, 20.00, 0.00),
    (81.77, 20.00, 0.00, -63.23, 20.00, 0.00),
    (81.77, 20.00, 0.00, -63.23, 20.00, 0.00),
    (81.77, 20.00, 0.00, -63.23, 20.00, 0.00),
    (81.77, 20.00, 0.00, -63.23, 20.00, 0.00),
    (81.77, 20.00, 0.00, -63.23, 20.00, 0.00),
    (81.77, 20.00, 0.00, -63.23, 20.00, 0.00),
)
CY7_B = (
    (429.84, 114.62, 119.61, -0.16, 0.00, 0.04),
    (428.96, 114.16, 117.54, -1.24, 0.00, 0.04),
    (428.18, 113.91, 116.69, -2.32, 0.00, 0.04),
    (426.03, 113.74, 114.69, -5.43, 0.00, 0.04),
    (422.54, 113.57, 111.73, -10.13, 0.00, 0.04),
    (418.23, 113.34, 107.74, -15.80, 0.00, 0.04),
    (413.50, 113.02, 103.78, -21.68, 0.00, 0.04),
    (337.44, 91.01, 84.17, -26.96, 0.00, 0.04),
    (279.73, 74.51, 69.45, -30.93, 0.00, 0.04),
    (139.20, 31.06, 36.18, -31.37, 3.64, 0.04),
    (176.38, 40.58, 35.19, -31.89, 6.91, 0.04),
    (268.21, 47.21, 39.46, -32.83, 9.82, 0.04),
    (228.50, 36.76, 25.87, -34.79, 12.36, 0.04),
    (192.03, 29.74, 16.17, -38.10, 14.55, 0.04),
    (159.15, 25.27, 9.50, -42.91, 16.36, 0.04),
    (131.22, 22.57, 5.09, -48.13, 17.82, 0.04),
    (110.01, 21.07, 2.34, -52.20, 18.91, 0.04),
    (95.00, 20.32, 0.76, -55.74, 19.64, 0.04),
    (86.18, 20.00, 0.04, -58.82, 20.00, 0.04),
    (85.68, 20.00, 0.04, -59.32, 20.00, 0.04),
    (84.92, 20.00, 0.04, -60.08, 20.00, 0.04),
    (84.13, 20.00, 0.04, -60.87, 20.00, 0.04),
    (83.40, 20.00, 0.04, -61.60, 20.00, 0.04),
    (82.76, 20.00, 0.04, -62.24, 20.00, 0.04),
    (82.25, 20.00, 0.04, -62.75, 20.00, 0.04),
    (81.86, 20.00, 0.04, -63.14, 20.00, 0.04),
    (81.60, 20.00, 0.04, -63.40, 20.00, 0.04),
    (81.49, 20.00, 0.04, -63.52, 20.00, 0.04),
    (81.45, 20.00, 0.04, -63.55, 20.00, 0.04),
    (81.44, 20.00, 0.04, -63.56, 20.00, 0.04),
    (81.44, 20.00, 0.04, -63.56, 20.00, 0.04),
    (81.43, 20.00, 0.04, -63.57, 20.00, 0.04),
    (81.43, 20.00, 0.04, -63.57, 20.00, 0.04),
    (81.43, 20.00, 0.04, -63.57, 20.00, 0.04),
    (81.43, 20.00, 0.04, -63.57, 20.00, 0.04),
    (81.43, 20.00, 0.04, -63.57, 20.00, 0.04),
    (81.43, 20.00, 0.04, -63.57, 20.00, 0.04),
    (81.43, 20.00, 0.04, -63.57, 20.00, 0.04),
    (81.43, 20.00, 0.04, -63.57, 20.00, 0.04),
)

# the speed-ramp position-gap rule (6c) camera, Link-following centre chase: himo2 (r58a, keyed pair, N 2, Link swings on
# the rope, hungCamera style 35: centre follow 0.5 xz / 0.25 y) frames 174-191, the camera centre's frame-end xyz (A, B)
# (frame 180: B 6.0 behind in z, 2.4 in y; the per-axis term g(0.5) |v_x|, g(0.25) |v_y|, g(0.5) |v_z| covers it)
HIMO2_A = (
    (-3.649, 95.377, -303.089),
    (-3.280, 89.534, -301.375),
    (-2.720, 81.203, -297.781),
    (-2.074, 72.724, -291.628),
    (-1.406, 65.880, -281.820),
    (-0.751, 62.184, -266.862),
    (-0.129, 63.085, -244.783),
    (0.455, 70.159, -212.957),
    (1.008, 85.365, -167.658),
    (1.555, 111.641, -102.860),
    (1.724, 119.281, -70.089),
    (1.899, 124.602, -41.834),
    (2.059, 128.843, -15.886),
    (2.208, 132.881, 8.301),
    (2.345, 137.245, 30.660),
    (2.472, 142.247, 51.264),
    (2.586, 147.952, 69.828),
    (2.688, 154.401, 86.595),
)
HIMO2_B = (
    (-2.494, 96.261, -303.413),
    (-2.241, 90.330, -301.667),
    (-1.903, 82.724, -298.807),
    (-1.468, 73.931, -293.577),
    (-0.992, 66.139, -284.939),
    (-0.505, 61.165, -271.377),
    (-0.023, 60.727, -250.808),
    (0.454, 66.717, -220.290),
    (0.941, 81.716, -175.100),
    (1.545, 115.502, -94.937),
    (1.682, 121.018, -71.015),
    (1.863, 125.959, -44.729),
    (2.026, 129.764, -19.809),
    (2.176, 133.327, 3.916),
    (2.315, 137.211, 26.153),
    (2.443, 141.727, 46.731),
    (2.559, 147.005, 65.534),
    (2.663, 153.052, 82.527),
)

# the speed-ramp position-gap rule (6c) camera, centre chase: pkk1d_4 (keyed pair, N 4, Link standing, locked on to a
# Bokoblin from frame 31) frames 28-80, the camera centre's frame-end xyz (A, B) relative to A's frame 28; the centre
# chases the lock-on camera's Link-to-target point (frame 54: 2.19 apart within the time slack, cam 2)
PKK1D4_A = (
    (0.000, 0.000, 0.000),
    (0.000, 0.000, 0.000),
    (0.000, 0.000, 0.000),
    (0.000, 0.000, 0.000),
    (-0.032, -5.164, 0.134),
    (-0.554, -9.769, 0.780),
    (-2.401, -13.440, 1.347),
    (-5.185, -15.851, 0.444),
    (-7.420, -17.038, -2.153),
    (-8.273, -17.319, -5.422),
    (-7.972, -17.082, -8.395),
    (-7.079, -16.635, -10.710),
    (-6.032, -16.183, -12.408),
    (-4.968, -15.858, -13.621),
    (-4.152, -15.804, -14.217),
    (-3.602, -16.013, -14.411),
    (-3.266, -16.432, -14.362),
    (-3.085, -16.999, -14.143),
    (-2.990, -17.397, -13.715),
    (-2.929, -17.984, -13.437),
    (-2.902, -18.368, -13.311),
    (-2.891, -18.626, -13.261),
    (-3.186, -17.006, -14.126),
    (-4.374, -12.877, -18.975),
    (-5.835, -6.782, -24.426),
    (-7.444, 1.135, -29.396),
    (-9.281, 10.744, -33.205),
    (-10.497, 17.329, -34.273),
    (-11.310, 19.614, -33.780),
    (-11.864, 17.165, -32.870),
    (-12.147, 10.692, -32.000),
    (-11.905, 2.624, -30.757),
    (-11.116, -2.751, -28.621),
    (-10.370, -6.435, -26.692),
    (-9.745, -8.858, -25.279),
    (-9.228, -10.089, -24.374),
    (-8.779, -10.844, -23.999),
    (-8.377, -11.442, -24.020),
    (-7.976, -12.021, -24.270),
    (-7.555, -12.583, -24.591),
    (-7.178, -13.089, -24.912),
    (-6.957, -13.382, -25.242),
    (-7.529, -13.079, -25.344),
    (-8.096, -11.971, -24.620),
    (-7.359, -10.629, -22.663),
    (-4.547, -11.416, -20.407),
    (-1.826, -14.569, -18.040),
    (-0.336, -16.967, -15.785),
    (3.892, -18.235, 5.047),
    (7.604, -22.026, 23.894),
    (10.686, -16.358, 40.912),
    (13.785, -21.698, 56.416),
    (16.433, -17.783, 70.066),
)
PKK1D4_B = (
    (0.000, 0.000, 0.000),
    (0.000, 0.000, 0.000),
    (0.000, 0.000, 0.000),
    (0.000, 0.000, 0.000),
    (-0.008, -4.726, 0.054),
    (-0.266, -9.120, 0.504),
    (-1.581, -12.845, 1.223),
    (-4.151, -15.493, 0.944),
    (-6.720, -16.955, -1.121),
    (-8.105, -17.452, -4.274),
    (-8.198, -17.340, -7.422),
    (-7.507, -16.937, -9.990),
    (-6.516, -16.459, -11.901),
    (-5.489, -16.047, -13.274),
    (-4.547, -15.821, -14.178),
    (-3.869, -15.856, -14.572),
    (-3.436, -16.138, -14.656),
    (-3.187, -16.608, -14.536),
    (-3.059, -17.129, -14.247),
    (-2.989, -17.590, -13.855),
    (-2.946, -18.116, -13.621),
    (-2.931, -18.461, -13.525),
    (-3.014, -18.174, -13.741),
    (-3.551, -15.981, -15.694),
    (-4.812, -11.290, -20.696),
    (-6.311, -4.661, -25.998),
    (-7.985, 3.751, -30.631),
    (-9.646, 12.600, -33.576),
    (-10.748, 18.007, -34.168),
    (-11.505, 18.940, -33.600),
    (-12.017, 15.309, -32.810),
    (-12.179, 8.318, -31.918),
    (-11.739, 1.037, -30.311),
    (-10.952, -3.823, -28.215),
    (-10.258, -7.096, -26.480),
    (-9.686, -9.138, -25.262),
    (-9.212, -10.214, -24.575),
    (-8.798, -10.915, -24.383),
    (-8.412, -11.501, -24.538),
    (-8.021, -12.078, -24.871),
    (-7.619, -12.630, -25.248),
    (-7.295, -13.078, -25.614),
    (-7.315, -13.187, -25.911),
    (-7.911, -12.629, -25.793),
    (-8.093, -11.377, -24.697),
    (-6.631, -10.670, -22.575),
    (-3.770, -12.269, -20.258),
    (-1.379, -15.244, -17.839),
    (3.552, -17.288, 3.849),
    (7.598, -21.577, 23.575),
    (10.874, -16.073, 41.426),
    (14.108, -21.512, 57.685),
    (16.867, -17.704, 72.016),
)


def atan2s_t(dx, dz):
    return int(round(math.atan2(dx, dz) * 32768.0 / math.pi))


def write_run(d, n, s0=1000, steplog=True, proc_change=40, pos_err=0.0, extra_se=False, se_shift=0, new_prof=False,
              save_byte=None, nan_at=None, dt_one=False, admp_err=0.0, lag=0.0, pre=False, anim_loop=None, amb=None, noarc=False, cam_lag=0.0, cam_err=0.0, cam_err_at=60, foot=None, aim=None, anim_switch=None, rngdiv=None, rt=None, cse=None, cproc=None, accel=None, face=None, alead=None, ctr=None, fchg=None, myaw=None, kick=None, se_series=None, se_span=None, anim_lag=0.0, rstep=None, pstep=None, ctype=None, rngmode=None, flead=None, fchase=None, drawword=None, cchase=None, en=None, push=None, life=None, life_dump=True, ship=None, ride=None, rtse=None, amb_caller=None, mvbg=None, link_dt1=False, ccarry=None, chung=None, cbump=None, frames=FRAMES, procs=None):
    os.makedirs(d)
    ldm = bytearray()  # the strict RNG-pair rule (8d) addendum 3: Link's health (life.admp: mMaxLife, mLife), LIFE = [(from frame, value), ...]
    lv = lambda f: [v for f0, v in life if f >= f0][-1]
    k = n
    dt = 1.0 / n
    tr, cam, se, dg, rng, st = [], [], [], [], [], []
    adm = bytearray()
    edm = bytearray()  # the strict RNG-pair rule (8d): a converted RNG-driven enemy's dump (en.admp), see enemy
    x, v, a = 0.0, 0.0, 0.25  # Link: x += v*dt + a*dt*(1-dt)/2 after v += a*dt (exact arc: full steps equal A)
    p = 0
    # the aim-boost stop rule (6e) (aim boost): A carries the boosted mNormalSpeed into the next frame, B starts each step unboosted
    aim_a, aim_b = 14.4, 12.0
    D = lambda u, h: max(u - max(u * (1.0 - 0.5 ** h), 4.0 * h), 0.0)  # setNormalSpeedF's stop: rate 0.5, min step 4 (x dt)
    step = s0 - 5 * k  # some steps before the origin
    ks = {}  # the step-resolution threshold rule (6l): the kick route's state
    ps = {}  # 6l addendum (g): the position-driven speed step route's state
    cs = {}  # 6f chase form: the run's chased angle
    ls = [0.0, 0.0, 0]  # the animation-phase lead rule (6d(b)): the animation's frame, rate and "first body call pending"
    for f in range(-5, len(PMAN2_A) if myaw else frames):  # (the speed-ramp position-gap rule (6c) camera, yaw episodes: the pair's 125 frames)
        for j in range(k):
            full = int(j == k - 1)
            v += a * dt
            x += v * dt + (a * dt * (1 - dt) / 2 if dt < 1 and not noarc else 0.0)  # noarc: the speed-ramp position-gap rule (6c)'s ramp drift
            if f < 0 and pre:
                # before the origin (state load): sound starts and a short-lived process that differ between the runs
                if j == 0 and f >= -n:
                    se.append("%d %d %d %s 02123456" % (step, full, 1, "00004805"))
                if f == -2:
                    dg.append("%d 00000009 01C1 200 %016x 02300000 00000000 00000000 00000000" % (step, j))
            if f >= 0:
                if full and f == proc_change:
                    pass
                proc = 5 if (f > proc_change or (f == proc_change and full)) else 0
                if procs:  # [(frame, proc)]: Link's procedure changes (taken on the frame's last step)
                    proc = ([procs[0][1]] + [p_ for f0, p_ in procs if f > f0 or (f == f0 and full)])[-1]
                # lag: Link's position as A's trajectory LAG frames earlier (a decision moved by part of a frame)
                xx = x - v * lag + a * lag * lag / 2 + (pos_err if f == 60 else 0.0)
                if push is not None and f >= push:  # the strict RNG-pair rule (8d) addendum: Link pushed / hit differently from frame PUSH
                    xx += 30.0 * (f - push + 1)
                an = f + (j + 1) * dt - 1  # animation frame, rate 1 (looping: rate 2.5, as late as Link)
                if anim_loop:
                    an = (2.5 * (an - lag - anim_lag) + 10) % anim_loop
                if anim_switch:
                    # prope3 frames 65-151: B's animation leads LEAD frames at rate 1.1 (the proc body's first run one
                    # step after its init, the animation-phase lead rule (6d)); both switch to a new animation at frame S
                    S, ld = anim_switch
                    an = 1.1 * (an + (ld if n > 1 else 0.0)) if f < S else 1.1 * (an - S)
                rcol = 2.5 if anim_loop else 1.0
                if alead:
                    # the animation-phase lead rule (6d(b)), prope7's subject entry: at F0 an init (on the frame's first step: B's non-tick30 step)
                    # sets an animation at rate 0, the next body call replaces it at rate R; each step first plays the
                    # animation at the rate set before (frame += rate x dt), then the body runs: B leads one frame. KIND at
                    # frame S: "map" a new animation at rate 2 whose start frame is 0.5 x the old one's (the throw), "reset"
                    # one from frame 0.3 (B 1.5 ahead: an old lead kept across it); ERR frames of B's error from F0 on
                    F0, R, err, S, kind = alead
                    proc = 0x76 if f >= F0 else 0
                    if f < F0:
                        an = f + (j + 1) * dt
                    else:
                        if f == F0 and j == 0:
                            ls[:] = [0.0, 0.0, 1]
                        else:
                            ls[0] += ls[1] * dt
                            if ls[2]:
                                ls[1], ls[2] = R, 0
                        if kind and f == S and j == 0:
                            ls[0] = 0.5 * ls[0] if kind == "map" else (1.8 if n > 1 else 0.3)  # (0.3: not a loop wrap)
                            ls[1] = 2.0
                        if f == 85 and j == 0:  # both runs: a new animation from frame 0 before the end (the window
                            ls[:] = [0.3, 2.0, 0]  # has no A frame after the last one)
                        an = ls[0] + (err * R if n > 1 and f < 85 else 0.0)
                        rcol = ls[1]
                if rstep:
                    # the animation-phase lead rule (6d) addendum (2026-11-16): a running animation's rate changes. "prun": the pair's
                    # measured wait animation (N 4; test frame 10 = frame 254, continued at the end rates outside
                    # the data, loop 60). ("jump", F0, R0, R1, ERR): rate R0 -> R1 set in frame F0 (A plays F0 at R0 and
                    # R1 from F0 + 1; B sets R1 on F0's first step after its play, steps 2..N at R1); ERR frames of a real
                    # error of B (x R1) from F0 + 5 on
                    proc = 4
                    if rstep[0] == "prun":
                        assert n in (1, 4)
                        L, kf = len(PRUN4A_A), f - 10
                        u = 0.0 if n == 1 else (j + 1) * dt - 1.0  # the step's time from the frame's end
                        if kf < 0:
                            a0, r0 = PRUN4A_A[0] if n == 1 else PRUN4A_B[0][3]
                            an, rcol = (a0 + r0 * (kf + u)) % 60.0, r0
                        elif kf >= L:
                            a0, r0 = PRUN4A_A[L - 1] if n == 1 else PRUN4A_B[L - 1][3]
                            an, rcol = (a0 + r0 * (kf - L + 1 + u)) % 60.0, r0
                        else:
                            an, rcol = PRUN4A_A[kf] if n == 1 else PRUN4A_B[kf][j]
                    else:
                        F0, R0, R1, err = rstep[1:]
                        t = f + (j + 1) * dt
                        if n == 1:
                            an = 10.0 + R0 * min(t, F0 + 1) + R1 * max(0.0, t - F0 - 1)
                        else:
                            an = 10.0 + R0 * min(t, F0 + dt) + R1 * max(0.0, t - F0 - dt) + (err * R1 if f >= F0 + 5 else 0.0)
                        rcol = R1 if f >= F0 else R0
                xs = "nan" if (nan_at is not None and f == nan_at) else "%.3f" % xx
                sp, ns = v, v  # speedF = mNormalSpeed (no foot travel)
                if foot:
                    # the animation-phase lead rule (6d), pl3's strafe stop: speedF = mNormalSpeed 12 up to frame F0 - 1; from F0 on mNormalSpeed 3
                    # and the speed is foot travel S = dP/dphi of the animation phase phi (phi = 1 at the end of frame F0),
                    # S = 5 - cos(2 (phi - 1.2)): a dip at phi 1.2, just inside B's frame F0. A's frame speed is the chord of
                    # its frame, P(phi) - P(phi - 1); B's feet run LAM animation frames ahead and each step travels its own
                    # chord (P(phi) - P(phi - dt)) / dt, so B's frame-end step sits in the dip, below A's whole window
                    # (as pl3's 4.51 vs A's 5.25 / 6.08); ERR: a real speed error of B on top
                    F0, lam, err = foot
                    if f < F0:
                        sp, ns = 12.0, 12.0
                    else:
                        P = lambda q: 5.0 * q - __import__("math").sin(2.0 * (q - 1.2)) / 2.0
                        ph = (f - F0) + (j + 1) * dt + (lam if n > 1 else 0.0)
                        sp, ns = (P(ph) - P(ph - dt)) / dt + (err if n > 1 else 0.0), 3.0
                        an = ph
                if aim:
                    # prope3's stop in procRopeSubject (0x76): steady 14.4 (the 12 cap x 1.2) up to frame F0 - 1, then the
                    # stick released: A 8.64 / 5.18 / 1.42 / 0, B 7.2 / 2.4 / 0 (measured in prope3 frames 271-274);
                    # ERR: a real speed error of B on frame F0 + EF (EF 3: A's last decelerating frame, 1.42 -> 0, where
                    # the cap max |ns_A| bounds the carry term: B 3 units fast there fails)
                    F0, err, ef = aim
                    proc = 0x76
                    if f >= F0:
                        if n == 1:
                            aim_a = 1.2 * D(aim_a, 1.0)
                        else:
                            aim_b = D(aim_b, dt)
                    sp = ns = aim_a if n == 1 else 1.2 * aim_b + (err if f == F0 + ef else 0.0)
                zs, ang, tgt = 0.0, 16384, " - - - - - -"
                if accel:
                    # the turn-factor sampling rule (6g), pgrab1's second move start (frames 166-173, measured): Link turns and accelerates from
                    # frame F0 (proc 4 -> 6); A's mNormalSpeed and angle per frame, B's mNormalSpeed lagging by the per-step
                    # cos sampling (B's angles set to A's: only the speed effect); ERR added to B's speed on frame F0 + EF
                    F0, err, ef = accel
                    nsA = (0.87, 2.59, 4.94, 7.57, 10.48, 13.63, 16.95, 17.0)
                    nsB = (0.53, 1.99, 4.13, 6.81, 9.85, 13.08, 16.43, 17.0)
                    angA = (9803, 12557, 14742, 16620, 18150, 19336, 20161, 20853)
                    k0 = f - F0
                    proc = 4 if k0 < 0 else 6
                    sp = ns = 0.0 if k0 < 0 else (nsA if n == 1 else nsB)[min(k0, 7)] + (err if n > 1 and k0 == ef else 0.0)
                    ang = 6803 if k0 < 0 else angA[min(k0, 7)]
                    xs = "%.3f" % (sum((nsA if n == 1 else nsB)[min(q, 7)] for q in range(max(k0 + 1, 0))))
                if face:
                    # the target-facing turn rule (6f), pgrab2 (measured): Link walks and stops by frame 20 (mNormalSpeed 0 -> 16 -> 0: the 6c
                    # drift 0.25 x 32 = 8 lets the runs end 6.7 apart: A 627.2 0 345.9, B 629.2 0 339.4), waits (proc 4) and at
                    # F0 turns to the pot at 684 0 364 (procGrabReady 110, attention actor A recorded from F0 - 2): A 13114,
                    # B 11946 (each within 64 of atan2s from its own position). BERR: B's angle off by that from F0 + 3;
                    # STEER: at that frame A turns to 16384 (stops facing), B too (STEER > 0) or B keeps facing (< 0)
                    F0, berr, steer = face[:3]
                    lift, lerr, bturn = (tuple(face[3:]) + (False, 0.0, 0))[:3]
                    u = min(f, 20) / 20.0
                    vv = 16.0 * min(f, 8) / 8.0 if f <= 12 else max(0.0, 16.0 * (20 - f) / 8.0)
                    sp = ns = vv
                    gx, gz = (2.0 * u, -6.5 * u) if n > 1 else (0.0, 0.0)
                    xs, zs = "%.3f" % (627.2 - 200.0 * (1.0 - u) + gx), 345.9 + gz
                    proc = 6 if f < 20 else 4 if f < F0 else 110
                    if f >= F0:
                        ang = 13114 if n == 1 else 11946 + (berr if f >= F0 + 3 else 0)
                    if steer and f >= abs(steer) and (n == 1 or steer > 0):
                        ang = 16384
                    if bturn and n > 1 and f >= bturn:
                        ang += 600  # B turns, A does not: the held facing ends
                    if f >= F0 - 2:
                        tgt = " - - - 684.000 0.000 364.000"
                    if lift and f >= F0 + 5:
                        # pgrab4's lift (proc 111): the pot rises on Link's facing line, ~20 units a frame, B half a frame
                        # ahead (A y 11.8 / B 22.5 at 136); thrown away (no target) from F0 + 20 on; LERR: B's pot off in x
                        h = 20.0 * (f - (F0 + 5) + (0.5 if n > 1 else 0.0))
                        tgt = " - - - %.3f %.3f %.3f" % (684.0 + (lerr if n > 1 else 0.0), h, 364.0) if f < F0 + 20 else " - - - - - -"
                if flead:
                    # rule 2 (2026-11-17), keyed pair frame 161 in shape: Link walks +x at 10 / frame (no
                    # speed ramp: no 6c drift), B LEAD frames ahead (0.5: 5 units, inside link-tslack's window); from F0
                    # both face a target (attention actor A, slot 1) 20 ahead in x and 132 to the side that walks along
                    # at 9.2 / frame (it chases Link), B's TLEAD frames ahead (-0.5: 4.6 behind, inside the window), each
                    # from its own position: the two leads turn the facing direction by ~720 units (> 256) that A's
                    # slow-turning angle in the window does not take. BERR: B's angle off by that from F0 + 3; TERR: B's
                    # target off in x by that from F0 + 3
                    F0, lead, tlead, berr, terr = flead
                    tau0 = f - 1 + (j + 1) * dt
                    lx = 10.0 * (tau0 + (lead if n > 1 else 0.0))
                    xs, zs = "%.3f" % lx, 0.0
                    sp = ns = 10.0
                    if f >= F0 - 2:  # (the target is recorded from F0 - 2, as pgrab2's; the turn at F0)
                        qx = 10.0 * F0 + 20.0 + 9.2 * (tau0 + (tlead if n > 1 else 0.0) - F0) + (terr if n > 1 and f >= F0 + 3 else 0.0)
                        tgt = " - - - %.3f 0.000 132.000" % qx
                    if f >= F0:
                        ang = (atan2s_t(qx - lx, 132.0) + (berr if n > 1 and f >= F0 + 3 else 0)) % 65536
                rz = 0.0
                if mvbg and len(mvbg) > 3 and mvbg[3] and n > 1 and mvbg[0] <= f < mvbg[1]:
                    rz = mvbg[2] + mvbg[5]
                zs += rz
                fcols = ""
                if fchase:
                    # 6f chase form, keyed pair frames 157-163 in shape: Link walks +x at 10 /
                    # frame (B LEAD frames ahead), locked on to an actor at (10 F0 + 200, 0, 300) whose eye (trace columns
                    # 33-35) swings on a radius-40 circle 100 up (head animation, ~20 units / frame); from F0 each run chases
                    # its own Link-to-eye direction with setShapeAngleToAtnActor's rule (cLib_addCalcAngleS(2, 0x2000,
                    # 0x800)) at the start of each of its steps (A per frame, B per step at the converted rate), the trace
                    # records the angle at the step's end: A lags its eye by ~1 frame's motion, B by half. MODE from F0 + 3,
                    # B only: "wrong" B turns away 600 a frame, "stuck" B's chase stops
                    F0, lead, mode = fchase
                    lt = (lead if n > 1 else 0.0)
                    tau0 = f - 1 + (j + 1) * dt
                    lpos = lambda t: 10.0 * (t + lt)
                    def eye(t):
                        w = 0.5 * (t + lt)
                        return (10.0 * F0 + 200.0 + 40.0 * math.sin(w), 100.0, 300.0 + 40.0 * math.cos(w))
                    def edir(t):
                        e = eye(t)
                        return atan2s_t(e[0] - lpos(t), e[2])
                    xs, zs = "%.3f" % lpos(tau0), 0.0
                    sp = ns = 10.0
                    if "a" not in cs:
                        cs["a"] = 16384
                    if f >= F0 - 2:
                        if f < F0:
                            cs["a"] = edir(tau0)  # (locked on before F0: facing the eye at the frame's end)
                        elif mode and n > 1 and f >= F0 + 3:
                            if mode == "wrong" and j == k - 1:
                                cs["a"] = (cs["a"] - 600) % 65536
                        else:
                            g = (edir(tau0 - dt) - cs["a"] + 32768) % 65536 - 32768
                            if n == 1:
                                hs = int(g / 2)
                                if abs(hs) > 0x800:
                                    mv = max(-0x2000, min(0x2000, hs))
                                else:
                                    mv = g if abs(g) <= 0x800 else (0x800 if g > 0 else -0x800)
                            else:
                                mv = g * (1.0 - 0.5 ** dt)
                                mv = max(-0x2000 * dt, min(0x2000 * dt, mv))
                                if abs(mv) < 0x800 * dt:
                                    mv = g if abs(g) <= 0x800 * dt else (0x800 * dt if g > 0 else -0x800 * dt)
                                mv = int(round(mv))
                            cs["a"] = (cs["a"] + mv) % 65536
                        e = eye(tau0)
                        tgt = " %.3f 0.000 300.000 - - - - - -" % (10.0 * F0 + 200.0)
                        fcols = " - - - - - - %.3f %.3f %.3f" % e
                    ang = cs["a"]
                if cchase and cchase[0] == "lock" and f >= 31:
                    tgt = " 120.000 80.000 300.000 - - - - - -"  # (6c centre chase: the lock-on slot from pkk1d_4's frame 31)
                ys, vys = 0.0, 0.0
                if kick:
                    # the step-resolution threshold rule (6l) + addenda, / in shape, each run simulated
                    # at its own step (A at frame ends): Link moves at speedF 17; procFanGlide's init (proc 6) at frame F - 3
                    # (B: +BINIT) sets the animation frame 1.0 at rate 0.5 on that step (6d(b): B's later steps play on, A's
                    # do not: B 1.25 / 1.375 at the frame's end); the opening kick when the frame passes > 2.0 (strict: A
                    # at F, 2.0 at F - 1 is not; B on F - 1's second step) sets speedF 12, speed.y 15; then gravity 2.5 /
                    # frame and the gravity switch `speed.y < 2.5` -> 0.5 / frame (A takes a whole 2.5 step from 2.5 to 0:
                    # F + 5, switch at F + 6; B on its own steps), clamped at -2. VAR "gust*": a position-driven animation
                    # switch (a wind capsule's entry / exit: frame 10 then 20, held) at A's frames 66 / 72 and B's: "gust"
                    # 67 / 73 (late), "gustearly" 65 / 71, "gust2" 68 / 74 (two late), "gustseq" 67 / 73 to a different
                    # animation (15), "gustextra" 67 / 73 plus an extra switch (to 12) at 69. Frame 80: both change to
                    # proc 7 (a shared state change); default: both stand at 2000 / 1000 (the gap converged); VAR "fall"
                    # / "fallstay": both fall (speed.y -40) from their own heights (the 6l offset unconverged), B pushed
                    # 0.6 / frame in z (the wind push from its offset position), and land on y 1000 (proc 8): "fall"
                    # then stands on the same pad (2000, 0: the gap back), "fallstay" keeps its offset. VAR "init": the kick
                    # is a procedure change (6 -> 9) whose init sets the speed (6l addenda). ERR added to B's
                    # y at ERR_AT
                    F, binit, err, err_at = kick[:4]
                    var = kick[4] if len(kick) > 4 else ""
                    if not ks:
                        ks.update(x=0.0, y=1100.0, vy=0.0, sf=17.0, an=0.0, rate=1.0, glide=False, kicked=False, z=0.0, proc=5, fell=False)
                    sw = {"": (), "gust": (67, 73), "gustearly": (65, 71), "gust2": (68, 74), "gustseq": (67, 73), "gustextra": (67, 69, 73)}
                    swv = {"gustseq": (15.0, 20.0), "gustextra": (10.0, 12.0, 20.0)}
                    if var.startswith("gust") and j == 0:
                        frs = (66, 72) if n == 1 else sw[var]
                        vals = (10.0, 20.0) if n == 1 else swv.get(var, (10.0, 20.0))
                        if f in frs:
                            ks["an"], ks["rate"] = vals[frs.index(f)], 0.0
                    if f < 80:
                        if not ks["glide"] and f == F - 3 + (binit if n > 1 else 0) and j == 0:
                            ks.update(glide=True, an=1.0, rate=0.5, proc=6)
                        elif ks["glide"]:
                            ks["an"] += ks["rate"] * dt
                            if not ks["kicked"] and ks["an"] > 2.0:
                                ks.update(kicked=True, sf=12.0, vy=15.0)
                                if var == "init":  # the kick as a new procedure's init (the pair's CUT -> CUT_REVERSE)
                                    ks["proc"] = 9
                        else:
                            ks["an"] = f + (j + 1) * dt - 1
                        if ks["kicked"]:
                            g = -0.5 if ks["vy"] < 2.5 else -2.5
                            ks["vy"] = max(ks["vy"] + g * dt, -2.0)
                        ks["x"] += ks["sf"] * dt
                        ks["y"] += ks["vy"] * dt
                    elif var not in ("fall", "fallstay"):
                        ks.update(proc=7, x=2000.0, y=1000.0, vy=0.0, sf=0.0, rate=0.0)
                    else:
                        if ks["proc"] == 6:
                            ks.update(proc=7, vy=-40.0, sf=0.0, rate=0.0)
                        if ks["proc"] == 7:
                            ks["y"] += ks["vy"] * dt
                            ks["z"] += (0.6 if n > 1 else 0.0) * dt
                            if ks["y"] <= 1000.0:
                                ks.update(proc=8, y=1000.0, vy=0.0)
                                if var == "fall":
                                    ks.update(x=2000.0, z=0.0)
                    proc, sp, ns, vys, an, rcol = ks["proc"], ks["sf"], ks["sf"], ks["vy"], ks["an"], ks["rate"]
                    xs, ys, zs = "%.3f" % ks["x"], ks["y"], ks["z"]
                    if n > 1 and f == err_at:
                        ys += err
                if pstep:
                    # 6l addendum (g), 257/258 + 261/262 in shape: Link rolls at
                    # speedF 30; a position-driven speed step (the ground slope ahead) at x >= 1805 (-1.1) and x >= 1925
                    # (-2.55), each run testing its position at the start of each of its own steps (A at frame starts);
                    # B starts LEAD units ahead (13.3 = 0.44 frame, inside the +-0.5-frame time slack), so its
                    # steps come one frame early (B 60 / 64, A 61 / 65); ERR added to B's speed (a real speed error) from
                    # frame 75 on
                    lead, err = pstep[:2]
                    if not ps:
                        ps.update(x=(lead if n > 1 else 0.0), sf=30.0)
                    ps["sf"] = 30.0 - (1.1 if ps["x"] >= 1805.0 else 0.0) - (2.55 if ps["x"] >= 1925.0 else 0.0)
                    ps["x"] += ps["sf"] * dt
                    sp = ns = ps["sf"] + (err if n > 1 and f >= 75 else 0.0)
                    xs = "%.3f" % ps["x"]
                if fchg:
                    # the animation-phase lead rule (6d(c)), pgrab5 frames 115-122 (measured, trace columns 9, 17, 15, 16, 27-31) at frames F0..F0+7:
                    # the foot-travel stop with an animation change at F0 + 5 (pgrab5 120); KIND "chg" as measured,
                    # "nochg" the same with both animations played on from F0 + 5 (each run's frames shifted to continue:
                    # no change, the same speeds and toes), "foot1" B's base foot 1 on F0 + 5's first step; ERR added to
                    # both of B's step speeds on F0 + 5 (a real speed error of B's frame); column 32 m3598 as measured
                    # (pgrab6), "r05" 0.5 on every record, "nor" without the column (an older trace: r taken as 1)
                    F0, kind, err = fchg
                    assert n in (1, 2)
                    kf = f - F0
                    bf, toes, r3598 = 0, (-10.0, 0.0, 10.0, 0.0), 1.0
                    fol = kind.startswith("fol")  # the animation-phase lead rule (6d(c)) addendum: pfol1 frames 102-109 (PFOL1_A / PFOL1_B)
                    if fol and kf < 0:
                        # before the data: walking on (no foot travel: speedF = mNormalSpeed), the animation continued
                        # backwards at its rate (no change at F0), the toes of the first record (base foot 0)
                        rec = PFOL1_A[0]
                        sp, ns, rcol = rec[1], rec[1], rec[3]
                        an = rec[2] + rec[3] * (kf - (0.0 if n == 1 else 1.0 - (j + 1) * dt))
                        toes = rec[5:9]
                    if 0 <= kf < 8 and fol:
                        rec = PFOL1_A[kf] if n == 1 else PFOL1_B[kf][j]
                        sp, ns, an, rcol, bf = rec[:5]
                        toes = rec[5:9]
                        r3598 = rec[9]
                        if kf == 5 and n > 1:
                            sp += err
                            if kind == "folsame" and j == 0:
                                bf = 1  # B switches on the frame's first step as well: chord foot 1 as A's
                            if kind == "folaonly" and j == 1:
                                bf = 0  # B keeps foot 0 through the frame (switches on the next one): only A switches here
                        if kind == "folnosw":
                            bf = 0
                    elif 0 <= kf < 8:
                        rec = PGRAB5_A[kf] if n == 1 else PGRAB5_B[kf][j]
                        sp, ns, an, rcol, bf = rec[:5]
                        toes = rec[5:]
                        r3598 = 0.5 if kind == "r05" else PGRAB5_RA[kf] if n == 1 else PGRAB5_RB[kf][j]
                        if kf >= 5 and kind == "nochg":
                            an -= 3.92 if n == 1 else 3.557
                        if kf == 5 and n > 1:
                            sp += err
                            if kind == "foot1" and j == 0:
                                bf = 1
                    fcols = " - - - %d %.4f %.4f %.4f %.4f" % ((bf,) + tuple(toes))  # (target slot 3 unset: columns 24-26)
                    if kind != "nor":
                        fcols += " %.6f" % r3598
                tr.append("%d %d %.4f %d %.2f %d %s %.3f %.3f %.4f %.4f %.4f %.4f %d %d %.3f %.3f %.3f%s%s" % (
                    p, step, step / 60.0, full, 1.0 if dt_one or link_dt1 else dt, proc, xs, ys, zs, sp, sp, vys, 0.0, ang, ang, an, rcol, ns, tgt, fcols))
                # cam_lag: the eye trails the moving centre by cam_lag x its speed (the speed-ramp position-gap rule (6c) camera form); cam_err: eye off
                # (B only: the steady chase lag difference of the speed-ramp position-gap rule (6c)'s camera form is v / 6 at N = 2)
                ex = x - (cam_lag * v if n > 1 else 0.0)
                ey = 100.0 + (cam_err if f == cam_err_at else 0.0)  # (across the motion: the time slack cannot absorb it)
                ez, style, cx = -300.0, 0, x
                if myaw:
                    # the speed-ramp position-gap rule (6c) camera (yaw episodes): the view yaw / radius of frames 56-180 (measured, frame f =
                    # 56 + f) around a standing centre; KIND "" in the pair's styles (follow 90, manual 57 MM01 from
                    # 61), "follow" the same all in the follow style (no term), "still" the manual style with no stick
                    # (frame 56's yaw held); ERR deg added to B's view yaw from ERR_AT on (default 104 = 160, after
                    # both turns)
                    kind, err = myaw[:2]
                    err_at = myaw[2] if len(myaw) > 2 else 104
                    rec = (PMAN2_A if n == 1 else PMAN2_B)[max(f, 0)] if kind != "still" else PMAN2_A[0]
                    psi = math.radians(rec[0] + (err if n > 1 and f >= err_at else 0.0))
                    cx = 0.0  # (the centre stands, as in: Link stood still)
                    ex, ez = rec[1] * math.sin(psi), rec[1] * math.cos(psi)
                    style = 90 if kind == "follow" or (kind == "pman2" and f < 5) else 57
                cty, cz = 0, 0.0
                if ctype:
                    # 6l addendum (g) on the camera (214): the camera type changes from 0 to
                    # 1 (a position-driven decision) and the centre chases a new target H units away (0.75 per frame as the follow
                    # camera's chase, exact per step); A decides at the start of frame F, B LEAD frames earlier (1: its record
                    # shows the new type a frame early); ERR: a real centre error of B on frame F + AT (default 0)
                    F, H, lead, err = ctype[:4]
                    eat = ctype[4] if len(ctype) > 4 else 0
                    tau = f - 1 + (j + 1) * dt  # (frame f ends at tau = f)
                    t0 = F - 1 - (lead if n > 1 else 0.0)
                    if tau > t0 + 1e-9:
                        cty, cz = 1, H * (1.0 - 0.25 ** (tau - t0))
                    if n > 1 and f == F + eat:
                        cz += err
                cy, cmode = 50.0, 0
                if ccarry:
                    # 6l addendum (g) carry (131): at frame F the type changes 0 -> 1
                    # and the camera style to STYLE; the centre's y chases a new target H up with KEEP per frame (the
                    # style's centre follow); B decides LAG frames later inside the same frame (0.5: Link crosses in the
                    # frame's second step), so its chase starts later while x follows Link (the time slack cannot absorb
                    # the y lag); ERR units added to B's centre y on frame F + AT
                    F, H, lagc, keep, sty = ccarry[:5]
                    cerr, cat = (ccarry[5], ccarry[6]) if len(ccarry) > 6 else (0.0, 0)
                    tau = f - 1 + (j + 1) * dt
                    t0 = F - 1 + (lagc if n > 1 else 0.0)
                    if tau > t0 + 1e-9:
                        cty, style, cy = 1, sty, 50.0 + H * (1.0 - keep ** (tau - t0))
                    if n > 1 and f == F + cat:
                        cy += cerr
                if cchase:
                    # the speed-ramp position-gap rule (6c) camera, centre chase: pkk1d_4's measured centres (frames 28-80,
                    # held outside), the eye 50 up and 300 back of its centre. KIND "lock": Link's lock-on slot set and the
                    # camera in mode 2 (the lock-on camera) from frame 31, as measured; "follow": the same centres with no
                    # lock-on (the centre follows Link: no term). ERR units added to B's centre x on frame AT (default 54)
                    kind, err = cchase[:2]
                    eat = cchase[2] if len(cchase) > 2 else 54
                    rec = (PKK1D4_A if n == 1 else PKK1D4_B)[min(max(f - 28, 0), len(PKK1D4_A) - 1)]
                    cx, cy, cz = rec[0] + (err if n > 1 and f == eat else 0.0), 50.0 + rec[1], rec[2]
                    ex, ey, ez = cx, cy + 50.0, cz - 300.0
                    cmode = 2 if kind == "lock" and f >= 31 else 0
                if cbump:
                    # the per-step camera-collision rule (6b) episode: cy7's measured eye / centre (frames 196-234 as frames
                    # 30-68, held outside: A's first / last record in both runs); ERR units added to B's eye x from frame AT
                    # on (default None: none; 62 = cy7 228, the convergence frame: the gap then never converges)
                    err = cbump[0]
                    eat = cbump[1] if len(cbump) > 1 else 10 ** 6
                    i = f - 30
                    rec = CY7_A[0] if i < 0 else CY7_A[-1] if i >= len(CY7_A) else (CY7_A if n == 1 else CY7_B)[i]
                    ex, ey, ez, cx, cy, cz = rec
                    if n > 1 and f >= eat:
                        ex += err
                if chung:
                    # the speed-ramp position-gap rule (6c) camera, Link-following centre chase: himo2's measured centres
                    # (frames 174-191 as frames 40-57, held outside) in hungCamera style 35 (or STYLE), the eye 50 up and
                    # 300 back of its centre; ERR units added to B's centre x on frame AT (default 46 = himo2 180)
                    err = chung[0]
                    eat = chung[1] if len(chung) > 1 else 46
                    style = chung[2] if len(chung) > 2 else 35
                    # (after the data: both hold A's last centre; the measured frames end while the centre still moves)
                    rec = HIMO2_A[-1] if f - 40 >= len(HIMO2_A) else (HIMO2_A if n == 1 else HIMO2_B)[max(f - 40, 0)]
                    cx, cy, cz = rec[0] + (err if n > 1 and f == eat else 0.0), rec[1], rec[2]
                    ex, ey, ez = cx, cy + 50.0, cz - 300.0
                cam.append("%d %d %d %.2f %d %.3f %.3f %.3f %.3f %.3f %.3f %.3f" % (p, step, full, dt, 1, ex, ey, ez + rz, cx, cy, rz + cz, 60.0))
                # mvbg (the carrier-offset rule (6j) detection): (F0, F1, off): a moving BG carrier (process 00D3) under the player over
                # frames F0 .. F1 - 1 at x = 10 f (+ off in B)
                # (F0, F1, off, ride, conv, err) (the carrier-offset rule (6j), cap): the carrier's offset is across the motion (z) and Link
                # and the camera ride it (B's z + off, + err: a real error); conv: B's log lists the carrier as converted
                mv = " mvbg -" if mvbg else ""
                if mvbg and mvbg[0] <= f < mvbg[1]:
                    if len(mvbg) > 3 and mvbg[3]:
                        mv = " mvbg 00D3 %.3f 0.000 %.3f" % (10.0 * f, mvbg[2] if n > 1 else 0.0)
                    else:
                        mv = " mvbg 00D3 %.3f 0.000 0.000" % (10.0 * f + (mvbg[2] if n > 1 else 0.0))
                cam.append("    m100 0 m108 0 work 00000000 type %d mode %d style %d V 0" % (cty, cmode, style) + mv)
                dg.append("%d 00000001 00A8 6BC0 %016x 023D0000 00000000 00000000 00000000" % (step, hash((f, j)) & 0xFFFFFFFFFFFFFFFF))
                if full and 5 <= f <= 30:
                    dg.append("%d 00000002 00C0 B00 %016x 02100000 00000000 00000000 00000000" % (step, f))
                if full and new_prof and f == 50:
                    dg.append("%d 00000003 0123 400 %016x 02200000 00000000 00000000 00000000" % (step, f))
                if full:
                    rv = f * 7 + (1 if rngdiv is not None and f >= rngdiv else 0)  # the RNG-drift plausibility rule (8b): B's RNG differs from rngdiv on
                    rng.append("%d %08X %08X %08X c00000000" % (step, rv, f * 11, f * 13))
                    body = bytearray(0x40)
                    struct.pack_into(">f", body, 0x10, x + (admp_err if f == 70 else 0.0))
                    struct.pack_into(">I", body, 0x20, proc)
                    if drawword is not None:
                        # admp draw-written words (2026-11-17): +0x30 holds the time of the last draw pass before the
                        # record (A: the previous frame's, B: its earlier step's); +0x34 a logic word, B off by DRAWWORD
                        # from frame 70 on
                        struct.pack_into(">f", body, 0x30, f - 1.0 + (1.0 - dt if n > 1 else 0.0))
                        struct.pack_into(">i", body, 0x34, f + (int(drawword) if n > 1 and f >= 70 else 0))
                    adm += b"ADMP" + struct.pack("<QIfII", step, full, dt, 0x30001000, len(body)) + body
                    if life is not None:
                        lb = struct.pack(">HH", 0x2021, lv(f))  # mMaxLife: si.bin +0x20 below
                        ldm += b"ADMP" + struct.pack("<QIfII", step, full, dt, 0, len(lb)) + lb
                    if en is not None:
                        eb = enemy(f, rngdiv, **en)
                        edm += b"ADMP" + struct.pack("<QIfII", step, full, dt, 0x30002000, len(eb)) + eb
                elif drawword is not None and j == 0:
                    # B's first-step record (the draw before it: the previous frame's)
                    body = bytearray(0x40)
                    struct.pack_into(">f", body, 0x10, x)
                    struct.pack_into(">I", body, 0x20, proc)
                    struct.pack_into(">f", body, 0x30, f - 1.0)
                    struct.pack_into(">i", body, 0x34, f + (int(drawword) if f >= 70 else 0) - 1)
                    adm += b"ADMP" + struct.pack("<QIfII", step, full, dt, 0x30001000, len(body)) + body
                # the RNG-drift plausibility rule (8b): an unconverted RNG-timed actor (profile 01C1, d_a_npc_so) living frames rt[0]..rt[1] (None:
                # to the end), its jump sound at the start (caller 022E0284, unconverted code); cse: a sound of converted code
                # (B starts it on an in-between step); cproc: a converted profile (01DC, the camera) created at that frame
                if rt and full and rt[0] <= f and (rt[1] is None or f <= rt[1]):
                    dg.append("%d 00000007 01C1 600 %016x 022E0000 00000000 00000000 00000000" % (step, f))
                if rt and f == rt[0] and full:
                    se.append("%d %d %d %s 022E0284" % (step, full, 0, "00005939"))
                # rtse (the RNG-drift plausibility rule (8b) addendum): the frames of an unconverted RNG-timed sound (d_a_npc_so's jump, caller
                # 022E0194 modeJumpInit, as in pj2)
                if rtse and f in rtse and full:
                    se.append("%d %d %d %s 022E0194" % (step, full, 0, "00005938"))
                if cse is not None and f == cse and j == 0:
                    se.append("%d %d %d %s 0240D000" % (step, full, 1, "00006103"))
                if cproc is not None and full and f >= cproc:
                    dg.append("%d 00000008 01DC 900 %016x 02500000 00000000 00000000 00000000" % (step, f))
                # sounds: an event on some step of the frame
                for sf, sid in ((10, "00001234"), (41, "00005678")):
                    if f == sf + (se_shift if sid == "00005678" else 0) and j == 0:
                        se.append("%d %d %d %s 02123456" % (step, full, 1, sid))
                if f == 15 and j == 0:  # a wrapper passing a heap pointer: another address in each run
                    se.append("%d %d %d %08X 02123460" % (step, full, 4, 0x47C00000 + 8 * n))
                # ambient seagull cries (the sound-gate rule (8a)): A at 20/50/80; "moved" other frames, same count; "spam" every
                # frame from 30 to 45; "none" no cry in B
                cries = {None: (20, 50, 80), "moved": (24, 47, 83), "spam": (20,) + tuple(range(30, 46)) + (80,), "none": ()}[amb]
                # amb_caller: both records of a cry from that caller (pj2: 02520570 dComIfGp_getReverb, outside the gull's
                # code range: the pointer record is ambient only through its id record, compare60 se_keyf)
                if f in cries and j == 0:
                    se.append("%d %d %d %s %s" % (step, full, 0, "00004805", amb_caller or "02123456"))
                if f in cries and j == 0:  # its positioned start (wrapper 4, heap pointer) from kamome_auto_move
                    se.append("%d %d %d %08X %s" % (step, full, 4, 0x47D00000 + 4 * f, amb_caller or "02189530"))
                if extra_se and f == 70 and j == 0:
                    se.append("%d %d %d %s 02123456" % (step, full, 1, "00009999"))
                # a sustained per-frame sound series (one start a frame, frames S0..S1 in A); B's whole series SHIFT frames
                # off (sword pair: procCutRoll's spin sound one frame early); EXTRA: one more start in B
                if se_series and j == 0:
                    s0_, s1_, sh = se_series[:3]
                    fs = f - (sh if n > 1 else 0)
                    if s0_ <= fs <= s1_ or (n > 1 and len(se_series) > 3 and f == se_series[3]):
                        se.append("%d %d %d %s 02123456" % (step, full, 1, "00001059"))
                # the step-resolution threshold rule (6l) addendum (e): a per-frame series on frames S0..S1 of this run (psword2c's SWORD_CHARGE)
                if se_span and j == 0 and se_span[0] <= f <= se_span[1]:
                    se.append("%d %d %d %s 024424FC" % (step, full, 0, "0000203B"))
            st.append("%d %d %g" % (step, full, 1.0 if dt_one else dt))
            step += 1
            p += 1
    w = lambda name, ls: open(os.path.join(d, name), "w").write("\n".join(ls) + "\n")
    w("trace.txt", tr)
    w("cam.txt", cam)
    w("se.txt", se)
    w("digest.txt", dg)
    w("rng.txt", rng)
    if steplog and n > 1:
        w("steps.txt", st)
    w("log", ["[boot] ok"] + ([rngmode] if rngmode else []) + ["[test] origin at t=0, logic step %d" % s0] + (["[t60] test conversion profiles: 00A8,01DC,00D3"] if mvbg and len(mvbg) > 4 and mvbg[4] and n > 1 else []) + ["[test] end"])
    si = bytearray(range(256)) * 4
    if save_byte is not None:
        si[save_byte] ^= 0xFF
    if life is not None:
        struct.pack_into(">H", si, 0x22, lv(FRAMES - 1))  # mLife (the save area's dSv_info_c at +0x20)
        if life_dump:
            open(os.path.join(d, "life.admp"), "wb").write(bytes(ldm))
    if ctr:
        # the counter-edge rule (8c): an unconverted 30 Hz counter (u16 at OFF) counting down once a frame until frame STOP (its last
        # count), dumped every frame 40-80 (WWHD_SAVEINFO_AT names) and at the end; a converted actor's state word
        # (himo2.admp +0x3F8) changes at frame STATE (on B's first step of that frame; None: never)
        off, stop, state = ctr["off"], ctr.get("stop"), ctr.get("state")
        sw = ctr.get("sw61a")  # (freeze frame, resume frame, mTime resume frame or None): sw61a's measured series
        def put(f):
            b = bytearray(si)
            if sw:  # +0x86 counts down to the freeze (last count) and again from the resume; +0x44 0.020004 a frame
                fz, rs, ts = sw
                struct.pack_into(">H", b, 0x86, 25392 - min(f, fz) - max(0, f - rs + 1))
                struct.pack_into(">f", b, 0x44, 172.960175 + 0.020004 * (min(f, fz) + (max(0, f - ts + 1) if ts else 0)))
            elif ctr.get("fmt") == ">f":  # the time of day (+0x44 mTime): 0.02 a frame
                struct.pack_into(">f", b, off, 100.0 + 0.02 * min(f, stop))
            else:
                struct.pack_into(">H", b, off, 0x6300 - min(f, stop))
            return bytes(b)
        for f in (range(60, 360) if sw else range(40, 81)):
            open(os.path.join(d, "si.bin.%g" % (f / 30.0 - 0.005)), "wb").write(put(f))
        si = bytearray(put(frames))
        hm = bytearray()
        for f in range(frames):
            for j in range(k):
                body = bytearray(0x400)
                struct.pack_into(">i", body, 0x3F8, 5 if state is not None and f >= state else 2)
                hm += b"ADMP" + struct.pack("<QIfII", s0 + f * k + j, int(j == k - 1), dt, 0x10A00000, len(body)) + body
        open(os.path.join(d, "himo2.admp"), "wb").write(bytes(hm))
    open(os.path.join(d, "si.bin"), "wb").write(bytes(si))
    open(os.path.join(d, "link.admp"), "wb").write(bytes(adm))
    if en is not None:
        open(os.path.join(d, "en.admp"), "wb").write(bytes(edm))
    if ship:
        write_ship(d, n, ship)
    if ride:
        write_ride(d, n, ride)


def enemy(f, rngdiv, shift=0, stuck=None, nan=None, heal=None, bighit=None, spam=None, div=None):
    """the strict RNG-pair rule (8d): a converted RNG-driven enemy at frame f (body: action u8 +0, health s8 +1, timer s16 +2, pos f32
    +0x10): a 20-frame cycle walk (action 1, frames 0-11), attack (2, 12-15), wait (0, 16-19), the timer counting the cycle
    down, hit by 2 at cycle frames 30 and 60 of the route; B's RNG draws other choices from rngdiv on: its cycle SHIFT
    frames later (plausible); faults from a frame: STUCK (the walk state from there on), NAN (position), HEAL (health +3),
    BIGHIT (a hit of 6), SPAM (an attack every 4 frames); DIV: the choices differ from DIV on although the RNG does not
    (a keyed-RNG pair: a decision read Link's accepted-class offset, the strict RNG-pair rule (8d) addendum)"""
    rngdiv = div if div is not None else rngdiv
    g = f - shift if (rngdiv is not None and f >= rngdiv) else f
    c = g % 20
    act = 1 if c < 12 else 2 if c < 16 else 0
    if spam is not None and f >= spam:
        act = 2 if f % 4 < 2 else 1
    if stuck is not None and f >= stuck:
        act = 1
    hp = 8 - 2 * (g >= 30) - 2 * (g >= 60)
    if heal is not None and f >= heal:
        hp += 3
    if bighit is not None and f >= bighit:
        hp = min(hp, 8) - 6
    b = bytearray(0x40)
    struct.pack_into(">BbH", b, 0, act, hp, 20 - c)
    struct.pack_into(">f", b, 0x10, float("nan") if nan is not None and f >= nan else 5.0 * g)
    return bytes(b)


def cloth_noise(seed, frames, tc):
    """the cloth-simulation rule (6k) mean-position bound: a unit-variance AR(1) series of FRAMES values with correlation time TC frames
    from SEED"""
    rnd = random.Random(seed)
    out, x, p = [], rnd.gauss(0, 1), math.exp(-1 / tc)
    for f in range(frames):
        out.append(x)
        x = p * x + math.sqrt(1 - p * p) * rnd.gauss(0, 1)
    return out


def cloth_noise_flag(n, frames):
    """frame -> z offset per vertex [25] of gy13n4o's form (measured: runs s18_gy_tau.txt): the free columns move with a
    slow, shared swing (correlation ~150 frames, the same in A and B: driven by the game state) plus a chaotic flutter
    (8- and 40-frame components; independent in B: another sample); column 0 (the pole) holds rows 0 and 4, its middle
    rows flutter only (tau ~8, small sd). A: n 1, B: n > 1"""
    slow = cloth_noise(7, frames, 300.0)
    fa, ma_ = cloth_noise(11, frames, 8.0), cloth_noise(13, frames, 40.0)
    fb, mb_ = (cloth_noise(23, frames, 8.0), cloth_noise(29, frames, 40.0)) if n > 1 else (fa, ma_)
    out = []
    for f in range(frames):
        zs = []
        for v in range(25):
            c, r = v % 5, v // 5
            fl, md = fa[f], ma_[f]
            zs.append(0.0 if c == 0 and r in (0, 4) else 50.0 * fl if c == 0 else 40.0 * c * (2.5 * slow[f] + 0.5 * fl + 1.2 * md))
        out.append(zs)
    if n > 1:
        out2 = []
        for f in range(frames):
            zs = []
            for v in range(25):
                c, r = v % 5, v // 5
                fl, md = fb[f], mb_[f]
                zs.append(0.0 if c == 0 and r in (0, 4) else 50.0 * fl if c == 0 else 40.0 * c * (2.5 * slow[f] + 0.5 * fl + 1.2 * md))
            out2.append(zs)
        return out, out2
    return out, out


def cloth_series(n, mode, frames, conv=30):
    """write_cloth's per-frame (positions [75], speeds [75]) of a 5 x 5 flag; see write_cloth for MODE"""
    k, out = n, []
    w1, w2 = 0.37, 0.113
    noisy = mode.startswith("noise")
    if noisy:
        za, zb = cloth_noise_flag(n, frames)
    for f in range(frames):
        t, ph, sc = f, 0.0, 1.0
        off = 0.0
        if n > 1 and f > conv:
            if mode == "chaos":
                ph = 1.3
            elif mode == "fast":
                t, sc = conv + 2 * (f - conv), 2.0
            elif mode in ("stuck", "noisestuck"):
                t, sc = conv, 0.0
            elif mode.startswith("spike"):
                ph = 1.3
            elif mode.startswith("noiseoff"):
                off = float(mode[8:])
        pos, spd = [], []
        for v in range(25):
            c, r = v % 5, v // 5
            a = 40.0 * c
            u1, u2 = w1 * t + ph + 0.4 * r, w2 * t + 2 * ph + 0.7 * c
            if noisy:
                # B follows A up to the conversion, then its own flutter sample; t: frozen for noisestuck
                zz = za if n == 1 or f <= conv else zb
                tt = min(t, frames - 1)
                z, zp = zz[tt][v], zz[max(0, tt - 1)][v]
                # off: B's sail shifted by OFF x the vertex's sd (the sum of the components' variances) in z
                sdv = 0.0 if c == 0 and r in (0, 4) else 50.0 if c == 0 else 40.0 * c * math.sqrt(2.5 ** 2 + 0.5 ** 2 + 1.2 ** 2)
                pos += [100.0 * c, -80.0 * r, z + off * sdv]
                spd += [0.0, 0.0, sc * (z - zp)]
                continue
            pos += [100.0 * c, -80.0 * r + 0.3 * a * math.sin(u2), a * (math.sin(u1) + 0.5 * math.sin(u2))]
            spd += [0.0, sc * 0.3 * a * w2 * math.cos(u2), sc * a * (w1 * math.cos(u1) + 0.5 * w2 * math.cos(u2))]
        if mode.startswith("spike") and n > 1 and f == conv + 50:
            spd[72:75] = [0.0, 0.0, float(mode[5:]) * 160.0 * (w1 + 0.5 * w2) * math.hypot(1.0, 0.3)]
        out.append((pos, spd))
    return out


def write_cloth(d, n, mode, frames, conv=30):
    """the cloth-simulation rule (6k): a 5 x 5 flag's arrays as compare60's default cloth dump cl.admp (dCloth_packet_c pos[2] at 0 / 0x134,
    speed at 0x738, size 0x86C), one record per frame before Link's full-pass execute. Column 0 is held at the pole; the
    others flutter on two incommensurate waves growing with the column. B's flutter follows A's up to the conversion
    (frame CONV, logged as port t60.cpp does); after it, by MODE: "same" equal, "chaos" another phase (the per-vertex
    difference of a chaotic flutter at a finer step, same statistics: sf3's form), "fast" the waves at double speed,
    "stuck" frozen at the conversion frame with zero speed; "spikeX" chaos plus one sample (vertex 24, 50 frames after the
    conversion) at X x the flutter's speed bound (6k amendment: a single noisy max sample). The noise modes (6k bound)
    replace the waves by a random flutter with long episodes (cloth_noise; A one sample, B an
    independent one after the conversion: gy13n4o's form): "noise" alone, "noiseoffX" with B's sail shifted by X x its
    sd for the whole window, "noisestuck" frozen at the conversion frame"""
    k, out = n, bytearray()
    for f, (pos, spd) in enumerate(cloth_series(n, mode, frames, conv)):
        old = [pos[i] - spd[i] / k for i in range(75)]
        body = bytearray(0x86C)
        cur_at, old_at = (0, 0x134) if f % 2 == 0 else (0x134, 0)
        struct.pack_into(">75f", body, cur_at, *pos)
        struct.pack_into(">75f", body, old_at, *old)
        struct.pack_into(">75f", body, 0x738, *spd)
        out += b"ADMP" + struct.pack("<QIfII", 1000 + f * k + k - 1, 1, 1.0 / k, 0x10B00000, len(body)) + body
    open(os.path.join(d, "cl.admp"), "wb").write(bytes(out))
    if n > 1:
        open(os.path.join(d, "log"), "a").write("[t60] WWHD_T60_TEST_CONVERT_FROM: test conversion on from scenario time %.3f s\n" % (conv / 30.0))


def write_ride(d, n, ride):
    """the carrier-offset rule (6j): Link rides the ship of write_ship: (proc, err): Link's procedure (136 = daPyProc_SHIP_STEER, on
    board; another one: not on a carrier), his xz = the ship's at every step, the camera centre = the ship's position
    + 50 up and the eye 100 up / 300 behind; err = (frame, units): a real error of Link and the camera from that frame
    on (B only, +x); yerr (optional third item) = (frame, units): the same in y; Link's y = the ship's"""
    proc, err = ride[:2]
    yerr = ride[2] if len(ride) > 2 else None
    # ride[3] (optional, pairs): (R, STYLE, YERR_DEG, AT): a ride-style camera
    # whose eye sits R behind the centre along the ship's facing (its yaw follows the ship's heading), camera style
    # STYLE; YERR_DEG: a real yaw error of B's eye from frame AT on
    rcam = ride[3] if len(ride) > 3 else None
    pos, fac = {}, {}
    if rcam and len(rcam) > 4 and n > 1:
        # (R, STYLE, YERR_DEG, AT, (HD, HAT)): B's ship heading HD s16 units off A's from frame HAT on (an accepted
        # heading drift inside the ship gate's angle tolerance; the route's own arc keeps both headings equal)
        hd, hat = rcam[4]
        buf = bytearray(open(os.path.join(d, "ship.admp"), "rb").read())
        o = 0
        while o + 28 <= len(buf):
            step, full, dt, pr, size = struct.unpack_from("<QIfII", buf, o + 4)
            fr_ = (step - (1000 - 5 * n)) // n - 5
            if fr_ >= hat:
                v = struct.unpack_from(">h", buf, o + 28 + 0x32A)[0]
                struct.pack_into(">h", buf, o + 28 + 0x32A, (v + hd + 32768) % 65536 - 32768)
            o += 28 + size
        open(os.path.join(d, "ship.admp"), "wb").write(bytes(buf))
    data = open(os.path.join(d, "ship.admp"), "rb").read()
    o = 0
    while o + 28 <= len(data):
        step, full, dt, pr, size = struct.unpack_from("<QIfII", data, o + 4)
        pos[step] = struct.unpack_from(">fff", data, o + 28 + 0x314)
        fac[step] = struct.unpack_from(">h", data, o + 28 + 0x32A)[0]
        o += 28 + size
    def fix(name, f):
        out = []
        for l in open(os.path.join(d, name)).read().split("\n"):
            c = l.split()
            if len(c) > 8 and not l.startswith(" ") and int(c[1]) in pos:
                f(c, pos[int(c[1])], int(c[1]))
                l = " ".join(c)
            out.append(l)
        open(os.path.join(d, name), "w").write("\n".join(out))
    frame = lambda step: (step - (1000 - 5 * n)) // n - 5
    e = lambda step: err[1] if err and n > 1 and frame(step) >= err[0] else 0.0
    def tr(c, p, step):
        c[5] = str(proc)
        c[6], c[8] = "%.3f" % (p[0] + e(step)), "%.3f" % p[2]
        c[7] = "%.3f" % (p[1] - 50.0 + (yerr[1] if yerr and n > 1 and frame(step) >= yerr[0] else 0.0))
    def cam(c, p, step):
        c[5], c[6], c[7] = "%.3f" % (p[0] + e(step)), "%.3f" % 150.0, "%.3f" % (p[2] - 300.0)
        c[8], c[9], c[10] = "%.3f" % (p[0] + e(step)), "%.3f" % 100.0, "%.3f" % p[2]
        if rcam:
            R, sty, yerr_d, yat = rcam[:4]
            psi = fac[step] * math.pi / 32768 + (math.radians(yerr_d) if n > 1 and frame(step) >= yat else 0.0)
            c[5], c[7] = "%.3f" % (p[0] + e(step) - R * math.sin(psi)), "%.3f" % (p[2] - R * math.cos(psi))
    fix("trace.txt", tr)
    fix("cam.txt", cam)
    if rcam:  # the camera style on the second cam.txt line
        cp = os.path.join(d, "cam.txt")
        txt = re.sub(r"style \d+", "style %d" % rcam[1], open(cp).read())
        open(cp, "w").write(txt)


def write_ship(d, n, ship):
    """the ship-drift rule (6h): the ship's actor dump (ship.admp, daShip_c offsets of compare60 SHIP_OFF) of a sailing route: speedF
    ramps 0 -> 40 over frames 10-20, turns by ship[2] (s16 units per frame) over frames ship[0] .. ship[1] - 1, then runs
    straight; ship[3] = (frame, units[, "along"]): a real xz error from that frame on (B only; +x, or along B's heading); ship[4] (optional): the frame where an
    opposite turn (-ship[2]) that started at ship[1] ends. A (n = 1) moves each frame along the frame-end heading on the
    frame-end speed, B each step along its own heading on its own speed (the arc)."""
    t0, t1, rate, err = ship[:4]
    t2 = ship[4] if len(ship) > 4 and ship[4] is not None else t1
    # ship[5] (optional, the early ship-decision rule (6i)): (S, early): no ramp; the sail goes up (mode 2 -> 1) at frame S - 9 and the speed
    # steps 0 -> 40 at frame S; with early (B only) both decisions come one record frame earlier, on B's last step of
    # the frame before (the sail-up of pp2 / pp4: the first-step input rule (12a)'s first-step input + the process order), so B leads by half
    # a frame of motion from then on
    jump = ship[5] if len(ship) > 5 else None
    # ship[6] (optional, the ship-drift rule (6h) / 6i(i), pair): (S, T, early): no ramp; sail up (mode 1) from the start;
    # the speed steps 0 -> 40 at frame S (A: the whole frame; B: 20 on the frame's first step, 40 from its second: B 10
    # behind); at frame T the sail goes down (mode 2) and the ship coasts (x 0.9 a frame). A: mode 2 at T, the coast
    # from T + 1. B with early: mode 2 on T's FIRST step (the ship reads the unconverted rider's tick30 request on the
    # next frame's first step), the coast from T's second step (x 0.9^dt a step: B leads the coast by half a frame);
    # without: mode 2 on T's last step, the coast from T + 1
    stop = ship[6] if len(ship) > 6 else None
    # ship[7] (optional, on ship y in flight, pair): (F0, F1, gap, err, only_a): a jump in
    # the air (daSFLG_FLY_e) over frames F0 .. F1 - 1, an arc of 300 over the take-off height; B (n > 1) leaves the water
    # gap units higher (the pre-jump float gap: on the take-off frame F0 - 1 and through the flight) and is err units
    # higher at the arc's top half (a real error); only_a: B does not jump
    fly = ship[7] if len(ship) > 7 else None
    # ship[8] (optional, the early ship-decision rule (6i(ii)), crane pair): (S, late, gap): the ship stands still in mode 10 (crane)
    # lowering the rope 2 segments a frame from frame 25 (one a step in B, CRANE_WORK once a frame: wrapper-4 record of
    # caller 025DFA24 + id 0x204D); at frame S the unconverted event system switches it to mode 11 (crane up): A for the
    # whole frame S (rope - 1), B with late on S's LAST step (its step 0 still lowers: rope + 1, its work sound on step
    # 0: B's rope 2 longer), without late on S's first step (the same rope as A). In mode 11 the rope winds 1 a frame
    # (CRANE_WORK each frame it changes, caller 0247FC0C, CRANE_STOP 0x284E when it reaches 20); the hook lifts out
    # (CRANE_LIFTUP 0x381D + setRopePos's wrapper-4 record 02476B38) at S + 20, in B gap frames later
    crane = ship[8] if len(ship) > 8 else None
    # ship[9] (optional, crane pair's ship-y gap 4.2): (amp, phi): the sea moves,
    # mWaterY = amp sin(2 pi (f + phi_B) / 60), the ship floats 50 over it; B's sea runs phi frames ahead (A: 0), inside
    # A's band (rule (c) plausible) but up to amp 2 pi phi / 60 above / below A's
    sea = ship[9] if len(ship) > 9 else None
    # ship[10] (optional, the early ship-decision rule (6i) addendum, pairs): (C, E, err): the sail's branch change. The speed
    # starts at 40, from frame 20 chases a falling wind power at the cap 2 a frame; the wind power reaches 0 at time
    # C + 0.5 (inside frame C): from then the speed declines by 0.1 a frame. A decides on the frame (frame C still
    # chases), B per step (frame C's second step declines): B ends 0.95 higher, kept while both decline. From frame E
    # both chase back to 40 (cLib_addCalc 0.1 / max 2) and converge. err (B only): a real speed error (speedF / mFwdVel)
    # from frame err[0] on, err[1] units
    branch = ship[10] if len(ship) > 10 else None
    # ship[11] (optional, rule (c) addendum, 2026-11-16, pair): (F, crest, foff): B only, from frame F on
    # its sea under the ship is crest units higher (mWaterY, outside A's band: B's ship sits elsewhere on a wave crest)
    # and it floats on it with the float offset 50 + foff (foff 0: correct)
    crest = ship[11] if len(ship) > 11 else None
    # ship[12] (optional, rule (c) derived slope bound, 2026-11-16): g: both runs' sea under the ship slopes along
    # z, mWaterY = g x z (+ crest for B), the ship floats 50 over it
    slope = ship[12] if len(ship) > 12 else None
    # ship[13] (optional, the early ship-decision rule (6i(iii)), pair): (lag, delay, rest_err, mode): a
    # DEMO_MOVE arrival (decision on POSITION): the ship in `mode` sails along +z at 40, from frame 30 slows 4 a frame
    # to 2, and stops (speedF 0) on the first step whose start is within 10 of the target z (TARGET_Z). B only: lag
    # units further back along z (ramped in over frames 30-40, inside 6h's drift), so B arrives later by its own
    # position; delay: B ignores the arrival for that many more frames (a real late stop); rest_err: B's rest x off by
    # that much once stopped (a real rest error)
    arrive = ship[13] if len(ship) > 13 else None
    arr_stop, arr_late = False, 0
    if branch:
        v = 40.0
    rope, sel, crane_once = 20, [], None
    dt = 1.0 / n
    x = z = 0.0
    v, h = 0.0, 0.0
    mode = 2
    adm = bytearray()
    step = 1000 - 5 * n
    for f in range(-5, FRAMES):
        for j in range(n):
            full = int(j == n - 1)
            if stop:
                S, T, early = stop
                mode = 1
                if f == S:
                    v = 40.0 if n == 1 or j > 0 else 20.0
                elif S < f <= T:
                    v = 40.0 if f < T or n == 1 or not early or j == 0 else v * 0.9 ** dt
                elif f > T:
                    v = v * (0.9 if n == 1 else 0.9 ** dt)
                if f > T or (f == T and (n == 1 or early or full)):
                    mode = 2
            elif jump:
                sj = jump[0] - (1 if jump[1] and n > 1 else 0)
                if (f == sj and full) or f > sj:
                    v = 40.0
                if (f == sj - 9 and full) or f > sj - 9:
                    mode = 1
            elif arrive:
                mode = arrive[3]
                lagz = arrive[0] * min(1.0, max(0.0, (f - 30) / 10.0)) if n > 1 else 0.0
                if not arr_stop and ARRIVE_Z - (z - lagz) < 10.0:
                    if n > 1 and arr_late < arrive[1] * n:
                        arr_late += 1
                    else:
                        arr_stop = True
                if arr_stop:
                    v = 0.0
                elif f >= 30:
                    v = max(2.0, v - 4.0 * dt)
                else:
                    v = 40.0
            elif branch:
                tb = f + j * dt  # the step's start time (A: the frame's)
                if f >= branch[1]:
                    v += min(2.0, 0.1 * (40.0 - v)) * dt
                elif f >= 20:
                    v -= (2.0 if tb < branch[0] + 0.5 else 0.1) * dt
            elif 10 <= f < 20 and not crane:
                v += 4.0 * dt
            if t0 <= f < t1:
                h += rate * dt
            elif t1 <= f < t2:
                h -= rate * dt
            ang = h * math.pi / 32768.0
            x += v * dt * math.sin(ang)
            z += v * dt * math.cos(ang)
            body = bytearray(0x720)
            e = err[1] if err and n > 1 and f >= err[0] else 0.0
            if fly and not (fly[4] and n > 1) and fly[0] - 1 <= f < fly[1]:
                F0, F1, gap, ferr = fly[:4]
                y = 50.0 + (gap if n > 1 else 0.0)
                if f >= F0:
                    u = (f - F0 + 1.0) / (F1 - F0 + 1.0)
                    y += 300.0 * 4.0 * u * (1.0 - u) + (ferr if n > 1 and 0.25 <= u <= 0.75 else 0.0)
                    struct.pack_into(">I", body, 0x644, 0x41)  # daSFLG_FLY_e | daSFLG_JUMP_e
                struct.pack_into(">fff", body, 0x314, x + e, y, z)
            elif err and len(err) > 2 and err[2] == "y":  # the ship's y off the sea (float offset outside A's range)
                struct.pack_into(">fff", body, 0x314, x, 50.0 + e, z)
            elif err and len(err) > 2 and err[2] == "along":  # along B's heading (ahead on the path)
                struct.pack_into(">fff", body, 0x314, x + e * math.sin(ang), 50.0, z + e * math.cos(ang))
            elif sea:
                wy = sea[0] * math.sin(2.0 * math.pi * (f + (j + 1) * dt - 1 + (sea[1] if n > 1 else 0.0)) / 60.0)
                struct.pack_into(">f", body, 0x6D8, wy)
                struct.pack_into(">fff", body, 0x314, x + e, 50.0 + wy, z)
            elif crest and n > 1 and f >= crest[0]:
                wy = crest[1] + (slope * z if slope else 0.0)
                struct.pack_into(">f", body, 0x6D8, wy)
                struct.pack_into(">fff", body, 0x314, x + e, 50.0 + wy + crest[2], z)
            elif arrive:
                lagz = arrive[0] * min(1.0, max(0.0, (f - 30) / 10.0)) if n > 1 else 0.0
                struct.pack_into(">fff", body, 0x314, x + (arrive[2] if n > 1 and arr_stop else 0.0), 50.0, z - lagz)
            elif slope:
                struct.pack_into(">f", body, 0x6D8, slope * z)
                struct.pack_into(">fff", body, 0x314, x + e, 50.0 + slope * z, z)
            else:
                struct.pack_into(">fff", body, 0x314, x + e, 50.0, z)
            struct.pack_into(">h", body, 0x322, ((int(round(h)) + 32768) % 65536) - 32768)
            struct.pack_into(">h", body, 0x32A, ((int(round(h)) + 32768) % 65536) - 32768)
            struct.pack_into(">f", body, 0x370, v)
            struct.pack_into(">f", body, 0x6E0, v)
            if branch and branch[2] and n > 1 and f >= branch[2][0]:
                struct.pack_into(">f", body, 0x370, v + branch[2][1])
                struct.pack_into(">f", body, 0x6E0, v + branch[2][1])
            if crane:
                S, late, gap = crane
                v = 0.0
                m11 = f > S or (f == S and (n == 1 or not late or full))
                mode = 11 if m11 else 10
                old_r = rope
                if f >= 25 and not m11:
                    rope += 2 if n == 1 else 1
                elif m11 and j == 0:  # 1 a frame (the share lands on the frame's first step in mode 11)
                    rope = max(20, rope - 1)
                if j == 0 and f >= 25 and not m11:
                    sel.append("%d %d 4 47C5EEC4 025DFA24" % (step, full))
                    sel.append("%d %d 0 0000204D 025DFA24" % (step, full))
                    crane_once = f
                if m11 and rope != old_r and rope > 20 and crane_once != f:
                    sel.append("%d %d 4 47C5EEC4 0247FC0C" % (step, full))
                    sel.append("%d %d 0 0000204D 0247FC0C" % (step, full))
                    crane_once = f
                if m11 and rope == 20 and old_r > 20:
                    sel.append("%d %d 0 0000284E 0247FC0C" % (step, full))
                if f == S + 20 + (gap if n > 1 else 0) and j == 0:
                    sel.append("%d %d 4 47C5E000 02476B38" % (step, full))
                    sel.append("%d %d 0 0000381D 02476B38" % (step, full))
                struct.pack_into(">h", body, 0x686, rope)
                struct.pack_into(">f", body, 0x370, 0.0)
                struct.pack_into(">f", body, 0x6E0, 0.0)
            body[0x635] = mode
            adm += b"ADMP" + struct.pack("<QIfII", step, full, dt, 0x30002000, len(body)) + body
            step += 1
    open(os.path.join(d, "ship.admp"), "wb").write(bytes(adm))
    if sel:
        open(os.path.join(d, "se.txt"), "a").write("\n".join(sel) + "\n")


# sv2 (KoRL's event camera, camera unconverted) frames 84-92: the eye seen from
# the centre (yaw s16, pitch s16, radius) in A and B; the camera latches its direction at 92 and holds it (A -14066, B
# -14154: the target's half-frame lead frozen into an 88 s16 offset, 2.5 units at R 300)
SV2_A = ((-10548, 1510, 188.55), (-11084, 1554, 215.48), (-11539, 1592, 245.50), (-11537, 1582, 287.07), (-11874, 1613, 288.80),
         (-12126, 1636, 290.09), (-12294, 1652, 290.95), (-12378, 1660, 291.38), (-14066, 1820, 300.0))
SV2_B = ((-10613, 1509, 190.34), (-11151, 1553, 218.31), (-11607, 1591, 249.51), (-11613, 1582, 287.08), (-11951, 1613, 288.80),
         (-12205, 1636, 290.09), (-12374, 1652, 290.95), (-12458, 1660, 291.38), (-14154, 1820, 300.0))


def post_cam(d, n, rec=None, style=None, unconv=False, ctype=None):
    """rewrite a synthetic run's cam.txt: rec(c, frame) edits a record's columns (eye 5-7, centre 8-10) in place;
    style(frame) -> the camera style of the frame's second line; ctype(frame) -> its camera type (default kept);
    unconv: every record dt 1.00 (the camera unconverted)"""
    cp = os.path.join(d, "cam.txt")
    out, fr = [], None
    for l in open(cp).read().split("\n"):
        c = l.split()
        if len(c) > 8 and not l.startswith(" "):
            fr = (int(c[1]) - (1000 - 5 * n)) // n - 5
            if rec:
                rec(c, fr)
            if unconv:
                c[3] = "1.00"
            l = " ".join(c)
        elif (style or ctype) and fr is not None and "style" in l:
            if style:
                l = re.sub(r"style \d+", "style %d" % style(fr), l)
            if ctype:
                l = re.sub(r"type \d+", "type %d" % ctype(fr), l)
        out.append(l)
    open(cp, "w").write("\n".join(out))


def post_contact(d, n, extra=0.0, gap=1.8, face=200, axis=0, spec=True):
    """the speed-ramp position-gap rule (6c) contact propagation (kmoD1_2 46-52): Link stands at the origin and an
    enemy (en.admp, fopAc words: pos +0x314, speedF +0x370, shape_angle.y +0x32A) stands 300 away on -x; over frames 46-52
    the enemy pushes Link 1 unit a frame (A) along +x (axis 0: radial, away from the enemy; axis 2: along +z, not
    radial), B (n > 1) pushed (7 + EXTRA) / 7 a frame, its enemy GAP further on -x and FACE s16 off A's facing. Link's
    speed columns stay 0 (the push is no own motion). spec: writes en.admp.spec next to the run (pos 6c, a16 facing)"""
    tp = os.path.join(d, "trace.txt")
    out, cnt, recs = [], collections.Counter(), []
    lines_ = open(tp).read().split("\n")
    per = collections.Counter()
    for l in lines_:
        c = l.split()
        if len(c) >= 18:
            per[(int(c[1]) - (1000 - 5 * n)) // n - 5] += 1
    for l in lines_:
        c = l.split()
        if len(c) < 18:
            out.append(l)
            continue
        step = int(c[1])
        f = (step - (1000 - 5 * n)) // n - 5
        k = per[f]
        j = cnt[f]
        cnt[f] += 1
        tau = f - 1 + (j + 1) / float(k)
        x = min(max(tau - 45.0, 0.0), 7.0) * ((7.0 + extra) / 7.0 if n > 1 else 1.0)
        c[6], c[7], c[8] = ("%.4f" % x, "0.000", "0.000") if axis == 0 else ("0.0000", "0.000", "%.4f" % x)
        c[9:13] = ["0.0000"] * 4
        out.append(" ".join(c))
        recs.append((step, int(j == k - 1), 1.0 / k))
    open(tp, "w").write("\n".join(out))
    dm = b""
    for step, full, dt in recs:
        body = bytearray(0x380)
        struct.pack_into(">fff", body, 0x314, -300.0 - (gap if n > 1 else 0.0), 0.0, 0.0)
        struct.pack_into(">h", body, 0x32A, 16384 + (face if n > 1 else 0))
        dm += b"ADMP" + struct.pack("<QIfII", step, full, dt, 0x30003000, len(body)) + bytes(body)
    open(os.path.join(d, "en.admp"), "wb").write(dm)
    if spec:
        open(os.path.join(d, "en.admp.spec"), "w").write("314 f32 2.0 6c=370\n318 f32 2.0\n31C f32 2.0 6c=370\n370 f32 0.5\n32A a16 256\n")


# the process-order read rule (14i) interim (a) (pair frame 50): the Bokoblin's eye relative to Link's
# position (trace columns 33-35 minus Link's pos) before its jump (A 49), B's eye at 50 (the jump-start pose after the
# Bokoblin's first step) and A's eye at 51 (A's Link reads the Bokoblin's frame-50 end a frame later)
KBK_EYE_BEFORE = (-48.074, 56.101, -220.573)
KBK_EYE_B50 = (-64.326, 91.145, -226.660)
KBK_EYE_A51 = (-63.321, 129.850, -237.505)


def post_trace_eye(d, n, F, b_off=0):
    """the process-order read rule (14i) interim (a): rewrite a synthetic run's trace.txt into a lock-on on a later-in-line enemy whose eye
    jumps: the lock-on slot (columns 18-20, the actor's position) and its eye (columns 33-35) recorded on every line, the
    shape angle (column 13) = cLib_targetAngleY(pos, eye). A (n = 1): KBK_EYE_BEFORE up to frame F, KBK_EYE_A51 from F + 1;
    B: the jump-start eye KBK_EYE_B50 on frame F, KBK_EYE_A51 from F + 1. b_off: s16 added to B's shape angle on frame F
    (B no longer faces its own eye)"""
    tp = os.path.join(d, "trace.txt")
    out = []
    for l in open(tp).read().split("\n"):
        c = l.split()
        if len(c) < 18:
            out.append(l)
            continue
        f = (int(c[1]) - (1000 - 5 * n)) // n - 5
        rel = KBK_EYE_BEFORE if f < F or (f == F and n == 1) else KBK_EYE_B50 if f == F else KBK_EYE_A51
        px, py, pz = float(c[6]), float(c[7]), float(c[8])
        eye = (px + rel[0], py + rel[1], pz + rel[2])
        a = int(round(math.atan2(rel[0], rel[2]) * 32768.0 / math.pi))
        if n > 1 and f == F:
            a += b_off
        c[13] = str(((a + 32768) % 65536) - 32768)
        c = c[:18] + ["%.3f" % eye[0], "0.000", "%.3f" % eye[2]] + ["-"] * 6 + ["-"] * 6 + ["%.3f" % v for v in eye]
        out.append(" ".join(c))
    open(tp, "w").write("\n".join(out))


def sv2_cam(table, f0, held_err=0.0, drift=0.0):
    """rec for post_cam: the eye from table (sv2 frames 84-92 at test frames f0 .. f0 + 8, held after; A's first view
    before) around a
    standing centre (0, 82, 0); held_err: s16 added to the held yaw (from the latch on); drift: s16 per frame the held yaw
    keeps moving after the latch (no hold)"""
    def rec(c, f):
        i = min(max(f - f0, 0), len(table) - 1)
        yaw, pit, R = table[i] if f >= f0 else SV2_A[0]  # (both runs stand at A's first view before the sweep)
        if f >= f0 + len(table) - 1:
            yaw += held_err + drift * (f - (f0 + len(table) - 1))
        psi, th = yaw * math.pi / 32768.0, pit * math.pi / 32768.0
        cx, cy, cz = 0.0, 82.0, 0.0
        c[5], c[6], c[7] = "%.3f" % (cx + R * math.cos(th) * math.sin(psi)), "%.3f" % (cy + R * math.sin(th)), "%.3f" % (cz + R * math.cos(th) * math.cos(psi))
        c[8], c[9], c[10] = "%.3f" % cx, "%.3f" % cy, "%.3f" % cz
    return rec


def ship_pitch(d, n, amp, period):
    """write shape_angle.x (0x328) into a synthetic ship.admp: B (n > 1) pitches amp sin(2 pi f / period) s16 off A (0)"""
    path = os.path.join(d, "ship.admp")
    buf = bytearray(open(path, "rb").read())
    o = 0
    while o + 28 <= len(buf):
        step, full, dt, pr, size = struct.unpack_from("<QIfII", buf, o + 4)
        f = (step - (1000 - 5 * n)) // n - 5
        struct.pack_into(">h", buf, o + 28 + 0x328, int(round(amp * math.sin(2.0 * math.pi * f / period))) if n > 1 else 0)
        o += 28 + size
    open(path, "wb").write(bytes(buf))


def write_evcam(d, n, on_ship, err=None, typ=3, sty=4, eyes=None):
    """the carrier-offset rule (6j) addendum: turn write_ride's camera into an event camera (type TYP,
    style STY on cam.txt's second line). on_ship: the centre stays write_ride's (the ship's position + 50 up: the
    target is the ship); A's eye stays write_ride's (300 behind, 50 above the centre), B's eye is A's at the same frame
    (EYES, A's frame -> eye as returned for A): it does not take B's ship offset (the pair's pull-back eye); else the centre
    and the eye both fixed far off the ship (the pair's fixed shot from frame 587). err = (frame, units): a real error of
    B's eye (+x) from that frame on. Returns frame -> eye"""
    cp = os.path.join(d, "cam.txt")
    frame = lambda step: (step - (1000 - 5 * n)) // n - 5
    out, got = [], {}
    for l in open(cp).read().split("\n"):
        c = l.split()
        if len(c) >= 12 and not l.startswith(" "):
            f = frame(int(c[1]))
            e = err[1] if err and n > 1 and f >= err[0] else 0.0
            if eyes is not None and f in eyes:
                c[5], c[6], c[7] = ("%.3f" % v for v in eyes[f])
            c[5] = "%.3f" % (float(c[5]) + e)
            got[f] = tuple(float(x) for x in c[5:8])
            if not on_ship:
                c[8], c[9], c[10] = "5000.000", "100.000", "5000.000"
                c[5], c[6], c[7] = "%.3f" % (5000.0 + e), "400.000", "4600.000"
            l = " ".join(c)
        out.append(l)
    txt = re.sub(r"type \d+", "type %d" % typ, "\n".join(out))
    open(cp, "w").write(re.sub(r"style \d+", "style %d" % sty, txt))
    return got


def cmp(a, b, *extra, end="3"):
    r = subprocess.run([sys.executable, os.path.join(HERE, "compare60.py"), a, b, end] + list(extra), capture_output=True, text=True)
    gates = {}
    for l in r.stdout.splitlines():
        parts = l.split()
        if len(parts) >= 2 and parts[1] in ("PASS", "FAIL", "SKIP"):
            if gates.get(parts[0]) != "FAIL":  # a gate printed once per dump (admp): any FAIL counts
                gates[parts[0]] = parts[1]
    return r.returncode, gates, r.stdout


def main():
    if len(sys.argv) == 4 and sys.argv[1] == "--fake-run":  # the stand-in runner's run
        write_run(sys.argv[2], int(sys.argv[3]))
        return 0
    base = os.environ.get("T60_TEST_TMP") or None
    tmp = tempfile.mkdtemp(prefix="compare60_test-", dir=base)
    fails = []
    try:
        A = os.path.join(tmp, "a30")
        write_run(A, 1)
        # keyed proofs (the sound-gate rule (8a) class, --keyed-proof): compare files of a keyed pair in the evKon10 form
        def kp(name, mode="A keyed seq 1, B keyed seq 1", strict="yes", rng="seeds equal at 1351 of 1351 frames", amb="PASS"):
            d = os.path.join(tmp, name)
            os.makedirs(d)
            open(os.path.join(d, "compare.txt"), "w").write("\n".join([
                "rng-mode    SKIP  " + mode,
                "sound       PASS  3880 / 3880 starts, unmatched A 0, B 0 (slack 1)",
                "sound-amb   %s  1 / 1 ambient starts, 1 key(s)" % amb,
                "rng         SKIP  report: " + rng,
                "equivalent (strict): " + strict, "equivalent: " + strict, "compare exit 0", ""]))
            return d
        KP_OK = kp("kp_ok")
        KP_UNKEYED = kp("kp_unkeyed", mode="A normal, B normal")
        KP_NOSTRICT = kp("kp_nostrict", strict="no (failed: link-motion)")
        KP_RNG = kp("kp_rng", rng="seeds equal at 1200 of 1351 frames")
        cases = [
            ("N=2 with steps.txt", dict(n=2), [], set()),
            ("N=4 with steps.txt", dict(n=4), [], set()),
            ("N=2 without steps.txt (K from dt)", dict(n=2, steplog=False), [], set()),
            ("N=2 origin on another step number", dict(n=2, s0=5000), [], set()),
            ("N=2 with records before the origin (load start)", dict(n=2, pre=True), [], set()),
            ("procedure change 1 frame late", dict(n=2, proc_change=41), [], set()),
            ("sound 1 frame late", dict(n=2, se_shift=1), [], set()),
            ("procedure change 3 frames late", dict(n=2, proc_change=43), [], {"link-procs"}),
            ("position 5 units off (within the time slack)", dict(n=2, pos_err=5.0), [], {"link-motion"}),
            ("position 40 units off", dict(n=2, pos_err=40.0), [], {"link-motion", "link-tslack"}),
            ("speed ramp without the xz arc term (rule note 6c drift)", dict(n=2, noarc=True), [], {"link-motion", "camera"}),
            ("eye trailing the moving centre by v/6 (rule note 6c camera drift)", dict(n=2, cam_lag=1.0 / 6.0), [], {"camera"}),
            ("eye drift plus 40 units off", dict(n=2, cam_lag=1.0 / 6.0, cam_err=40.0), [], {"camera", "cam-tslack"}),
            ("eye drift plus a late error just above the bounded tolerance", dict(n=2, cam_lag=1.0 / 6.0, cam_err=CAM_LATE_ERR, cam_err_at=85), [], {"camera", "cam-tslack"}),
            ("ramp drift plus a position 40 units off", dict(n=2, noarc=True, pos_err=40.0), [], {"link-motion", "link-tslack", "camera"}),
            ("Link 0.3 frame late (time slack passes)", dict(n=2, lag=0.3), [], {"link-motion"}),
            ("Link 1.5 frames late", dict(n=2, lag=1.5), [], {"link-motion", "link-tslack"}),
            ("extra sound", dict(n=2, extra_se=True), [], {"sound"}),
            ("sound 3 frames late", dict(n=2, se_shift=3), [], {"sound"}),
            ("ambient cries at other frames, same count (rule note 8a)", dict(n=2, amb="moved"), [], set()),
            ("ambient cry on every frame", dict(n=2, amb="spam"), [], {"sound-amb"}),
            ("ambient cry missing", dict(n=2, amb="none"), [], {"sound-amb"}),
            ("ambient count difference + keyed proof (8a class)", dict(n=2, amb="none"), ["--keyed-proof", KP_OK], set()),
            ("ambient count difference + keyed proof file (8a class)", dict(n=2, amb="none"), ["--keyed-proof", os.path.join(KP_OK, "compare.txt")], set()),
            ("ambient count difference + unkeyed proof", dict(n=2, amb="none"), ["--keyed-proof", KP_UNKEYED], {"sound-amb"}),
            ("ambient count difference + non-strict proof", dict(n=2, amb="none"), ["--keyed-proof", KP_NOSTRICT], {"sound-amb"}),
            ("ambient count difference + proof with RNG divergence", dict(n=2, amb="none"), ["--keyed-proof", KP_RNG], {"sound-amb"}),
            ("ambient count difference + missing proof", dict(n=2, amb="none"), ["--keyed-proof", os.path.join(tmp, "nope")], {"sound-amb"}),
            ("ambient cry every frame + keyed proof (never accepted)", dict(n=2, amb="spam"), ["--keyed-proof", KP_OK], {"sound-amb"}),
            ("new process profile", dict(n=2, new_prof=True), [], {"procs"}),
            ("save byte", dict(n=2, save_byte=0x123), [], {"save"}),
            ("save byte, masked", dict(n=2, save_byte=0x123), ["--save-mask", "120:8"], set()),
            ("NaN in Link trace", dict(n=2, nan_at=20), [], {"health"}),
            ("mode not active (B at dt 1)", dict(n=2, dt_one=True), [], {"mode"}),
        ]
        spec = os.path.join(tmp, "link.spec")
        open(spec, "w").write("10 f32 0.01   # pos x\n20 u32 0     # proc\n")
        cases += [("actor dump within spec", dict(n=2), ["--admp-spec", spec], set()),
                  ("actor dump word out of spec", dict(n=2, admp_err=1.0), ["--admp-spec", spec], {"admp"})]
        # (esa): `OFF f32 TOL rel=R`: |B - A| <= max(TOL, R x max(|A|, |B|))
        rspec_ok, rspec_bad = os.path.join(tmp, "link-rel1.spec"), os.path.join(tmp, "link-rel2.spec")
        open(rspec_ok, "w").write("10 f32 0 rel=1\n20 u32 0\n")
        open(rspec_bad, "w").write("10 f32 0.01 rel=1e-9\n20 u32 0\n")
        cases += [("actor dump word within its relative tolerance (rel=)", dict(n=2, admp_err=1.0), ["--admp-spec", rspec_ok], set()),
                  ("actor dump word beyond its relative tolerance and floor (rel=)", dict(n=2, admp_err=1.0), ["--admp-spec", rspec_bad], {"admp"})]
        for i, (name, kw, extra, want) in enumerate(cases):
            B = os.path.join(tmp, "b%d" % i)
            write_run(B, **kw)
            rc, gates, out = cmp(A, B, *extra)
            failed = {g for g, s in gates.items() if s == "FAIL"}
            need = {"mode", "health", "link-procs", "link-motion", "link-tslack", "camera", "cam-tslack", "sound", "procs", "save"}
            missing = need - set(gates)
            strict_only, slack_only = {"link-motion", "camera"}, {"link-tslack", "cam-tslack"}
            want_rc = 0 if not (want - slack_only) else 3 if not (want - strict_only) else 1
            if failed != want or rc != want_rc or missing:
                fails.append("%s: failed gates %s (want %s), rc %d, missing %s\n%s" % (name, sorted(failed), sorted(want), rc, sorted(missing), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # admp draw-written words (2026-11-17, fallback-resume drawshift.py): a word whose B first-step record equals
        # A's frame record on every differing frame is the draw's one-pass shift: masked in the report; a logic word that
        # differs stays listed
        AD = os.path.join(tmp, "a30dw")
        write_run(AD, 1, drawword=0)
        for name, kw, want_in, want_out in (("draw-written word masked, logic words equal", dict(drawword=0), ["masked (B's first-step record = A's on every differing frame): 1 (+0x30:"], ["+0x30:", "+0x34:"]),
                                             ("draw-written word masked, a logic word off by 3 listed", dict(drawword=3), ["+0x34:", "masked (B's first-step record = A's on every differing frame): 1 (+0x30:"], [])):
            B = os.path.join(tmp, "bdw" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AD, B)
            line = next((l for l in out.splitlines() if l.startswith("admp")), "")
            logic = line.split("logic words differing most:")[1].split(";")[0] if "logic words differing most:" in line else ""
            if not all(w in line for w in want_in) or any(w in logic for w in want_out) or ("+0x34:" in logic) != (kw["drawword"] != 0):
                fails.append("%s: admp line %r" % (name, line))
            else:
                print("ok   %s" % name)
        # (sea 0x26 seapk, 2026-11-21): a WWHD_MEM_DUMP region is no actor: no fopAc tevStr at +0x110, so its
        # words there are judged by the spec as logic words (no tevStr mask, no admp-light); an actor dump (its records
        # carry the process address at +0x24 / +0x40, base_process_class's tags) keeps the tevStr classification
        def mem_dump(d, name, proc, actor, err_at=None):
            src = open(os.path.join(d, "link.admp"), "rb").read()
            out, o = bytearray(), 0
            while o + 28 <= len(src):
                step, full, dt, _p, size = struct.unpack_from("<QIfII", src, o + 4)
                o += 28 + size
                body = bytearray(0x300)
                lx = struct.unpack_from(">f", src, o - size + 0x10)[0]  # Link's x of this record: equal in A and B per frame
                struct.pack_into(">I", body, 0x110, int(lx * 10) & 0xFFFF)  # a counter (mCounters' form)
                struct.pack_into(">I", body, 0x114, 1 + (1 if err_at is not None and step >= err_at else 0))
                struct.pack_into(">f", body, 0x194, 100.0 + lx)  # a "light" position word
                if actor:
                    struct.pack_into(">I", body, 0x24, proc)
                    struct.pack_into(">I", body, 0x40, proc)
                out += b"ADMP" + struct.pack("<QIfII", step, full, dt, proc, len(body)) + body
            open(os.path.join(d, name), "wb").write(bytes(out))
        mspec = os.path.join(tmp, "mem.spec")
        open(mspec, "w").write("110 u32 0\n114 u32 0\n")
        for mi, (name, actor, err, want_admp, want_light) in enumerate((
                ("mem dump: spec words at +0x110.. judged as logic, equal (no tevStr mask, no admp-light)", False, False, "PASS", None),
                ("mem dump: a spec word at +0x114 off by one is a FAIL (not masked as tevStr light)", False, True, "FAIL", None),
                ("actor dump (self pointers at +0x24 / +0x40): the tevStr classification stays (admp-light gate)", True, False, None, "any"))):
            AM, BM = os.path.join(tmp, "a30mem%d" % mi), os.path.join(tmp, "bm%d" % mi)
            write_run(AM, 1)
            write_run(BM, 2)
            mem_dump(AM, "seam.admp", 0x1046D8D0, actor)
            mem_dump(BM, "seam.admp", 0x1046D8D0, actor)
            if err:  # B's word off from its 20th record on
                buf = bytearray(open(os.path.join(BM, "seam.admp"), "rb").read())
                o, k = 0, 0
                while o + 28 <= len(buf):
                    size = struct.unpack_from("<I", buf, o + 24)[0]
                    if k >= 40:
                        struct.pack_into(">I", buf, o + 28 + 0x114, 2)
                    o += 28 + size
                    k += 1
                open(os.path.join(BM, "seam.admp"), "wb").write(bytes(buf))
            rc, gates, out = cmp(AM, BM, "--admp-spec", "seam=" + mspec)
            al = [l for l in out.splitlines() if l.split()[:1] == ["admp"] and "seam.admp" in l]
            ll = [l for l in out.splitlines() if l.split()[:1] == ["admp-light"] and "seam.admp" in l]
            ok = bool(al) and (want_admp is None or al[0].split()[1] == want_admp) and (bool(ll) if want_light else not ll)
            if want_admp == "PASS" and al and "tevStr" in al[0].split("worst")[0]:
                ok = False
            if not ok:
                fails.append("%s: admp %r, admp-light %r" % (name, al[:1], ll[:1]))
            else:
                print("ok   %s" % name)
        # the counter-edge rule (8c): a listed unconverted counter (forest-water timer +0x86) may differ by one
        # count per start / stop edge at a converted party's state change; not more, not elsewhere
        for ci, (name, ka, kb, want) in enumerate((
                ("8c: counter stops 1 frame early at a converted state change (prope10 form)",
                 dict(off=0x86, stop=60, state=60), dict(off=0x86, stop=59, state=59), set()),
                ("8c: a 2-count gap with one edge", dict(off=0x86, stop=60, state=60), dict(off=0x86, stop=58, state=58), {"save"}),
                ("8c: a field not on the list at |d| = 1", dict(off=0x90, stop=60, state=60), dict(off=0x90, stop=59, state=59), {"save"}),
                ("8c: the edge far from any converted change", dict(off=0x86, stop=60, state=30), dict(off=0x86, stop=59, state=30), {"save"}),
                # 8c fall sequence (sw61a): Link's fall change one frame early in B (A 174 /
                # B 173), the world freezes 8 frames later (forest-water timer +0x86 and mTime +0x44 stop: A after
                # frame 182, B after 181), no Link change between
                ("8c fall: the freeze 7 frames after Link's change, both a frame early in B (sw61a form, +0x86)",
                 dict(off=0x86, stop=57, proc=50), dict(off=0x86, stop=56, proc=49), set()),
                ("8c fall: the same for the time of day +0x44 (0.02 a frame, one step)",
                 dict(off=0x44, fmt=">f", stop=57, proc=50), dict(off=0x44, fmt=">f", stop=56, proc=49), set()),
                ("8c fall: +0x44 two steps off", dict(off=0x44, fmt=">f", stop=57, proc=50), dict(off=0x44, fmt=">f", stop=55, proc=49), {"save"}),
                ("8c fall: B's freeze later though Link's change is earlier", dict(off=0x86, stop=57, proc=50), dict(off=0x86, stop=58, proc=49), {"save"}),
                ("8c fall: Link's change not shifted", dict(off=0x86, stop=57, proc=50), dict(off=0x86, stop=56, proc=50), {"save"}),
                ("8c fall: the freeze beyond fall_seq (33 frames after the change)", dict(off=0x86, stop=75, proc=42), dict(off=0x86, stop=74, proc=41), {"save"}))):
            AC, BC = os.path.join(tmp, "a30ctr%d" % ci), os.path.join(tmp, "bctr%d" % ci)
            pa_, pb_ = ka.pop("proc", 40), kb.pop("proc", 40)
            write_run(AC, 1, ctr=ka, proc_change=pa_)
            write_run(BC, 2, ctr=kb, proc_change=pb_)
            rc, gates, out = cmp(AC, BC)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or rc != (1 if want else 0):
                fails.append("%s: failed gates %s (want %s), rc %d\n%s" % (name, sorted(failed), sorted(want), rc, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # 8c fall sequence, sw61a's numbers: Link's procedure
        # changes (A 39 at 174, B at 173; 40 / 37 / 4 at 210 / 239 / 259 in both), +0x86 last counts A 182 / B 181 and
        # again from 259, +0x44 (0.020004 a frame) stops at the same freeze and stays stopped to the end: save passes
        # (both edges of +0x86 and +0x44's stop tied); with Link's change unshifted the freeze is not tied: save fails
        SWP = [(0, 4), (75, 68), (77, 90), (109, 4), (127, 68), (129, 90), (148, 88), (158, 89), (174, 39), (210, 40), (239, 37), (259, 4)]
        for ci, (name, pb_, want) in enumerate((("8c fall: sw61a's numbers (freeze A 182 / B 181, resume 259, +0x44 one step)", 173, "PASS"),
                                                 ("8c fall: sw61a's numbers with Link's fall change unshifted", 174, "FAIL"))):
            AS, BS = os.path.join(tmp, "a30fall%d" % ci), os.path.join(tmp, "bfall%d" % ci)
            write_run(AS, 1, ctr=dict(off=0x86, sw61a=(182, 259, None)), procs=SWP, frames=362)
            write_run(BS, 2, ctr=dict(off=0x86, sw61a=(181, 259, None)), procs=[(f, p_) if f != 174 else (pb_, p_) for f, p_ in SWP], frames=362)
            rc, gates, out = cmp(AS, BS, end="12")
            sl = next((l for l in out.splitlines() if l.startswith("save")), "")
            if gates.get("save") != want or (want == "PASS" and "fall seq." not in sl):
                fails.append("%s: save %s (want %s)\n%s" % (name, gates.get("save"), want, out))
            else:
                print("ok   %s: %s" % (name, sl[:200]))
        # origin-clock (rule): the world clock at the scenario origin (port log line) must be equal
        def clock(d, t, sync):
            open(os.path.join(d, "log"), "a").write(
                ("[test] WWHD_TEST_SYNC_LOADS: 2 loader thread(s) of 2 names found\n" if sync else "") +
                "[test] WWHD_TEST_FIXED_SEED: scenario origin (WWHD_TEST_ORIGIN_RESEED): cM_rnd seeds 100 100 100 (were 1 2 3), "
                "particle seed 0 (was 0), mCounter0 %d (was %d, mTimer %d), logic step 1000\n" % (t + 96, t + 96, t))
        AO = os.path.join(tmp, "a30clk")
        write_run(AO, 1)
        clock(AO, 3174, True)
        for ci, (name, tb, want) in enumerate((("origin-clock: same world clock at the origin", 3174, set()),
                                               ("origin-clock: B's world a frame behind (async boot loads, pcr4)", 3173, {"origin-clock"}))):
            BO = os.path.join(tmp, "bclk%d" % ci)
            write_run(BO, 2)
            clock(BO, tb, True)
            rc, gates, out = cmp(AO, BO)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or rc != (1 if want else 0) or gates.get("origin-clock") not in ("PASS", "FAIL"):
                fails.append("%s: failed gates %s (want %s), rc %d\n%s" % (name, sorted(failed), sorted(want), rc, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # an animation loop wrap inside the time-slack window: interpolated on the unwrapped frame (loop length 32)
        AL = os.path.join(tmp, "a30loop")
        write_run(AL, 1, anim_loop=32)
        for name, kw, want in (("looping animation, Link 0.3 frame late (time slack passes)", dict(lag=0.3), {"link-motion"}),
                               ("looping animation, Link 1.5 frames late", dict(lag=1.5), {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "bloop%d" % len(want))
            write_run(B, 2, anim_loop=32, **kw)
            rc, gates, out = cmp(AL, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s (fails %s as it should)" % (name, ", ".join(sorted(want))))
        # the animation-phase lead rule (6d(b)) in link-motion (2026-11-16, kmoU frame 37: 31.61 reported from a wrap): B's looping
        # animation 0.5 frame ahead (position equal; A at 31.5, B at 32.0 -> 0) wraps a frame before A's: compared modulo the loop, every gate passes;
        # 1.5 frames ahead fails link-motion's anim (and only that)
        for name, kw, want in (("looping animation 0.5 frame ahead, wrap a frame early (link-motion modulo the loop)", dict(anim_lag=-0.2), set()),
                               ("looping animation 1.5 frames ahead", dict(anim_lag=-0.6), {"link-motion"})):
            B = os.path.join(tmp, "bloopw%d" % len(want))
            write_run(B, 2, anim_loop=32, **kw)
            rc, gates, out = cmp(AL, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # the animation-phase lead rule (6d): foot-travel speed from frame 55 on (A same route at 30 Hz); B's feet 0.46 animation frame
        # ahead (pl3's strafe stop) and per-step chords: passes link-tslack only with the 6d terms (without them it fails,
        # speed |d| above 0.5 at the dip); a real speed error of -1 on top fails it
        AF = os.path.join(tmp, "a30foot")
        write_run(AF, 1, foot=(55, 0.46, 0.0))
        for name, kw, want in (("foot-travel speed with pl3's phase lead and per-step chord (rule note 6d)", dict(foot=(55, 0.46, 0.0)), {"link-motion"}),
                               ("foot-travel phase lead plus a real speed error", dict(foot=(55, 0.46, -1.0)), {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "bfoot%d" % len(want))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AF, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # an animation's last frame before a new one with B 1 frame ahead at rate 1.1 (prope3 frame 150): passes
        # link-tslack (the old animation plays on at A's rate to the window's midpoint); 2 frames ahead fails
        AS = os.path.join(tmp, "a30sw")
        write_run(AS, 1, anim_switch=(70, 0.0))
        for name, kw, want in (("animation 1 frame ahead up to a new animation (prope3 frame 150)", dict(anim_switch=(70, 1.0)), {"link-motion"}),
                               ("animation 2 frames ahead", dict(anim_switch=(70, 2.0)), {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "bsw%d" % len(want))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AS, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s (fails %s as it should)" % (name, ", ".join(sorted(want))))
        # (sword pair): a per-frame sound series (frames 30-60 in A) that B runs one
        # frame early as a whole matches start for start within se_slack (maximum matching: the earliest free start in
        # the window, not the nearest, which left B's first and A's last start unmatched); two frames early, or one early
        # plus an extra start, fail
        ASE = os.path.join(tmp, "a30se")
        write_run(ASE, 1, se_series=(30, 60, 0))
        for name, kw, want in (("sound series one frame early (maximum matching within se_slack)", dict(n=2, se_series=(30, 60, -1)), set()),
                               ("sound series one frame early at N 4", dict(n=4, se_series=(30, 60, -1)), set()),
                               ("sound series one frame late", dict(n=2, se_series=(30, 60, 1)), set()),
                               ("sound series two frames early", dict(n=2, se_series=(30, 60, -2)), {"sound"}),
                               ("sound series one frame early plus one extra start", dict(n=2, se_series=(30, 60, -1, 20)), {"sound"})):
            B = os.path.join(tmp, "bse%d" % len(name))
            write_run(B, **kw)
            rc, gates, out = cmp(ASE, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # the step-resolution threshold rule (6l) addendum (e) (psword2c: SWORD_CHARGE B 54 / A 53): a per-frame series whose
        # start follows an accepted one-frame procedure shift (A changes at 29, B at 28; the series A 30-60, B 29-60) may
        # have one start more in B (shift +1 passes); two more (shift +2 of the series with a 1-frame procedure shift),
        # one more without a procedure shift, or one more at the edge away from the shift fail
        ASP = os.path.join(tmp, "a30sp")
        write_run(ASP, 1, proc_change=29, se_span=(30, 60))
        for name, kw, want in (("per-frame series one start more after a 1-frame procedure shift", dict(n=2, proc_change=28, se_span=(29, 60)), set()),
                               ("the same at N 4", dict(n=4, proc_change=28, se_span=(29, 60)), set()),
                               ("per-frame series two starts more after a 1-frame procedure shift", dict(n=2, proc_change=28, se_span=(28, 60)), {"sound"}),
                               ("per-frame series one start more without a procedure shift", dict(n=2, proc_change=29, se_span=(29, 60)), {"sound"}),
                               ("per-frame series one start more at the end (shift at the start)", dict(n=2, proc_change=28, se_span=(30, 61)), {"sound"})):
            B = os.path.join(tmp, "bsp%d" % (len(fails) * 100 + len(os.listdir(tmp))))
            write_run(B, **kw)
            rc, gates, out = cmp(ASP, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # the RNG-drift plausibility rule (8b): after the first RNG difference (frame 45) an unconverted RNG-timed actor 15 frames later passes
        # (procs-rng / sound-rng plausibility); a converted sound 3 frames late, a converted process 4 frames late, the
        # actor never deleted in B (stuck) or missing in B fail
        AR = os.path.join(tmp, "a30rng")
        write_run(AR, 1, rt=(50, 70), cse=60, cproc=60)
        for name, kw, want in (("RNG-timed unconverted actor 15 frames later after the RNG difference (rule note 8b)", dict(rngdiv=45, rt=(65, 85), cse=60, cproc=60), set()),
                               ("the same before the RNG difference (strict)", dict(rngdiv=None, rt=(65, 85), cse=60, cproc=60), {"procs", "sound"}),
                               ("converted sound 3 frames late after the RNG difference", dict(rngdiv=45, rt=(65, 85), cse=63, cproc=60), {"sound"}),
                               ("converted process 4 frames late after the RNG difference", dict(rngdiv=45, rt=(65, 85), cse=60, cproc=64), {"procs"}),
                               ("unconverted actor stuck (never deleted) after the RNG difference", dict(rngdiv=45, rt=(65, None), cse=60, cproc=60), {"procs-rng"}),
                               ("unconverted actor spawned late, alive at the cut 10 frames (< A's 20-frame life): not stuck", dict(rngdiv=45, rt=(80, None), cse=60, cproc=60), set()),
                               ("unconverted actor alive at the cut 2 x A's longest life (41 frames): stuck", dict(rngdiv=45, rt=(49, None), cse=60, cproc=60), {"procs-rng"}),
                               # the actor's one jump sound (A plays it once, after the RNG difference) is inside the whole remaining
                               # window of the 8b addendum: sound-rng tolerates it, procs still fails
                               ("unconverted actor missing in B", dict(rngdiv=45, rt=None, cse=60, cproc=60), {"procs"})):
            B = os.path.join(tmp, "brng%d" % len(name))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AR, B, "--converted", "00A8,01DC")
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # the strict RNG-pair rule (8d): a converted RNG-driven enemy (--plaus-spec) on a normal-RNG pair: after the first RNG difference
        # (frame 45) other choices (its cycle 7 frames later) pass actor-plaus; a stuck state, a NaN, health going wrong
        # (a rise, a hit of another strength) and an attack-rate blowup fail it; without an RNG difference it is skipped
        # (the strict gates); its sound starts (code range) after the difference are plausibility (sound-rng)
        pspec = os.path.join(tmp, "en.plaus")
        open(pspec, "w").write("dump en.admp\nprofile 00C0\ncode 02123000-02124000\nstate 0 u8\nhealth 1 s8\ntimer 2 u16\n"
                               "value 10 f32\nattack 2\n")
        AP = os.path.join(tmp, "a30plaus")
        write_run(AP, 1, en={})
        for name, kw, want in (("enemy with other RNG choices after the RNG difference (rule note 8d)", dict(rngdiv=45, en=dict(shift=7)), set()),
                               ("enemy stuck in one state", dict(rngdiv=45, en=dict(shift=7, stuck=48)), {"actor-plaus"}),
                               ("enemy position NaN", dict(rngdiv=45, en=dict(shift=7, nan=70)), {"actor-plaus"}),
                               ("enemy health rising", dict(rngdiv=45, en=dict(shift=7, heal=70)), {"actor-plaus"}),
                               ("enemy hit of another strength", dict(rngdiv=45, en=dict(shift=7, bighit=70)), {"actor-plaus"}),
                               ("enemy attack-rate blowup", dict(rngdiv=45, en=dict(shift=7, spam=46)), {"actor-plaus"}),
                               ("enemy, no RNG difference (strict gates; actor-plaus skipped)", dict(en={}), set())):
            B = os.path.join(tmp, "bplaus" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AP, B, "--plaus-spec", pspec, "--tol", "stuck=10", "--converted", "00A8,01DC,00C0")
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or "actor-plaus" not in gates or (("rngdiv" in kw) != (gates["actor-plaus"] != "SKIP")):
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the strict RNG-pair rule (8d) addendum: an enemy that pushes / hits Link, Link converted too, keyed RNG
        # (no RNG difference): Link and the enemy split at frame 50 (a decision read Link's accepted-class offset).
        # --strict-until 50 with a cause passes (actor-plaus from the cut); without the cut the strict gates fail; a cut
        # without a cause or with a cause naming no accepted rule fails strict-cut; a real divergence before the cut
        # (Link from 40, or the enemy's dump from 45) fails the strict gates
        CAUSE = "rule=6d decision=fight_run 4/2 reads Link's distance after the OSHI push"
        for name, kw, extra, want in (("Link and enemy split at 50, --strict-until 50 with a cause", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", CAUSE], set()),
                                      ("the same without the cut", dict(push=50, en=dict(shift=7, div=50)), [], {"link-motion", "link-tslack", "admp"}),
                                      ("the cut without a cause", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50"], {"strict-cut"}),
                                      ("the cut with a cause naming no accepted rule", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", "rule=9z decision=something else entirely"], {"strict-cut"}),
                                      ("a cause without the cut", dict(en={}), ["--cause", CAUSE], {"strict-cut"}),
                                      ("Link diverges at 40, before the cut at 50", dict(push=40, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", CAUSE], {"link-motion", "link-tslack"}),
                                      ("the enemy diverges at 45, before the cut at 50", dict(push=50, en=dict(shift=7, div=45)), ["--strict-until", "50", "--cause", CAUSE], {"admp"}),
                                      ("the cut, then a stuck enemy (actor-plaus from the cut)", dict(push=50, en=dict(shift=7, div=50, stuck=52)), ["--strict-until", "50", "--cause", CAUSE], {"actor-plaus"})):
            B = os.path.join(tmp, "bcut" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AP, B, "--plaus-spec", pspec, "--tol", "stuck=10", "--converted", "00A8,01DC,00C0", "--admp-spec", spec, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the strict RNG-pair rule (8d) addenda 2 / 3: a cause on the enemy's side (its own the per-step smoothing rule (3) float drift
        # feeding the knock-back on Link) passes; a cause naming no accepted rule (side=enemy rule=6d, rule=4a on the
        # enemy, an unknown side) fails strict-cut; after the cut the save's health may differ within A's range (A hit
        # 12 -> 10 at 60, B at 62, or no hit in B: 12 at the end), a save that differs in anything else fails save, health
        # outside A's range fails save-life (B 8), and so does a health difference before the cut (B hit at 45)
        ECAUSE = "side=enemy rule=3 decision=the jump attack's knock-back angle reads the ChuChu's float-drifted position"
        # 8d addendum 4: the first-step input rule (12a) (process order, unconverted Link) is a valid
        # Link-side cause only when Link is not converted in B
        LCAUSE12 = "side=link rule=12a decision=the guard decision on the first step sees Link's tick30-step cut one frame later"
        AL = os.path.join(tmp, "a30life")
        write_run(AL, 1, en={}, life=[(-99, 12), (60, 10)])
        for name, kw, extra, want in (("an enemy-side cause (rule 3 float drift -> knock-back)", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", ECAUSE], set()),
                                      ("an enemy-side cause naming a Link-only rule", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", "side=enemy rule=6d decision=the knock-back reads the push"], {"strict-cut"}),
                                      ("an enemy-side cause naming no rule", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", "side=enemy rule=9 decision=the knock-back reads something"], {"strict-cut"}),
                                      ("a Link-side rule 12a cause, Link unconverted (8d addendum 4)", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", LCAUSE12, "--converted", "01DC,00C0"], set()),
                                      ("a Link-side rule 12a cause with Link converted", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", LCAUSE12], {"strict-cut"}),
                                      ("a cause on an unknown side", dict(push=50, en=dict(shift=7, div=50)), ["--strict-until", "50", "--cause", "side=camera rule=3 decision=the knock-back reads something"], {"strict-cut"}),
                                      ("after the cut, the hit on Link 2 frames later", dict(push=50, en=dict(shift=7, div=50), life=[(-99, 12), (62, 10)]), ["--strict-until", "50", "--cause", ECAUSE], set()),
                                      ("after the cut, no hit on Link in B (12 within A's 10..12)", dict(push=50, en=dict(shift=7, div=50), life=[(-99, 12)]), ["--strict-until", "50", "--cause", ECAUSE], set()),
                                      ("after the cut, health only in the save blocks", dict(push=50, en=dict(shift=7, div=50), life=[(-99, 12), (62, 10)], life_dump=False), ["--strict-until", "50", "--cause", ECAUSE], set()),
                                      ("after the cut, the save differs in another byte too", dict(push=50, en=dict(shift=7, div=50), life=[(-99, 12), (62, 10)], save_byte=0x123), ["--strict-until", "50", "--cause", ECAUSE], {"save"}),
                                      ("after the cut, B's health outside A's range (8)", dict(push=50, en=dict(shift=7, div=50), life=[(-99, 12), (62, 8)]), ["--strict-until", "50", "--cause", ECAUSE], {"save-life"}),
                                      ("B's health differs before the cut (hit at 45)", dict(push=50, en=dict(shift=7, div=50), life=[(-99, 12), (45, 10)]), ["--strict-until", "50", "--cause", ECAUSE], {"save-life"}),
                                      ("no cut: a health difference fails the strict save", dict(life=[(-99, 12), (62, 8)]), [], {"save"})):
            B = os.path.join(tmp, "blife" + str(abs(hash(name))))
            kw.setdefault("life", [(-99, 12), (60, 10)])  # A's health where the case does not change it
            write_run(B, 2, **kw)
            ex = ["--plaus-spec", pspec, "--tol", "stuck=10", "--converted", "00A8,01DC,00C0", "--admp-spec", spec] if "en" in kw else []
            rc, gates, out = cmp(AL, B, *(ex + extra))
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (extra and gates.get("save-life") not in ("PASS", "FAIL")):
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the step-resolution threshold rule (6l) addendum (f): a hit outcome sampled at step resolution (a
        # converted attacker's hit volume checked on every step's Move) may land ONE frame early in B: `rule=6l hit=SPEC`
        # with the cut on B's hit frame passes when A makes the same hit on the next frame (A: Link's health 12 -> 10 at
        # 60) or on the same frame; two frames early, one frame late, a hit A does not have near it, or rule=6l without
        # hit= fail strict-cut
        HC = lambda side: "side=%s rule=6l hit=life decision=hit sampled at step resolution (the jump attack on Link)" % side
        for name, life, cut, extra_c, want in (("a hit one frame early in B (6l f, side=enemy)", [(-99, 12), (59, 10)], 59, HC("enemy"), set()),
                                               ("a hit one frame early in B (6l f, side=link)", [(-99, 12), (59, 10)], 59, HC("link"), set()),
                                               ("a hit on A's frame with a 6l cause", [(-99, 12), (60, 10)], 60, HC("enemy"), set()),
                                               ("a hit two frames early in B", [(-99, 12), (58, 10)], 58, HC("enemy"), {"strict-cut"}),
                                               ("a hit one frame late in B", [(-99, 12), (61, 10)], 61, HC("enemy"), {"strict-cut", "save-life"}),
                                               ("a hit in B with no counterpart in A", [(-99, 12), (40, 10)], 40, HC("enemy"), {"strict-cut"}),
                                               ("side=enemy rule=6l without hit=", [(-99, 12), (59, 10)], 59, "side=enemy rule=6l decision=hit sampled at step resolution", {"strict-cut"}),
                                               ("side=link rule=6l without hit= (M2's Link-side 6l cause)", [(-99, 12), (59, 10)], 59, "side=link rule=6l decision=fight_run reads Link's 6l-early roll", set())):
            B = os.path.join(tmp, "bhit" + str(abs(hash(name))))
            write_run(B, 2, push=cut, en=dict(shift=7, div=cut), life=life)
            rc, gates, out = cmp(AL, B, "--plaus-spec", pspec, "--tol", "stuck=10", "--converted", "00A8,01DC,00C0", "--admp-spec", spec,
                                 "--strict-until", str(cut), "--cause", extra_c)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the sound-gate rule (8a) (se_keyf, pair): a cry whose two records share a caller outside the gull's code
        # (dComIfGp_getReverb): moved cries pass (the pointer record shares the id's ambient class; without se_keyf it
        # would be a strict non-ambient source and fail `sound`); missing cries fail sound-amb
        AK = os.path.join(tmp, "a30ambk")
        write_run(AK, 1, amb_caller="02520570")
        for name, kw, want in (("ambient cries with the reverb caller at other frames (rule note 8a, se_keyf)", dict(amb="moved"), set()),
                               ("ambient cries with the reverb caller missing", dict(amb="none"), {"sound-amb"})):
            B = os.path.join(tmp, "bambk_%s" % "".join(ch for ch in name if ch.isalnum())[:60])
            write_run(B, 2, amb_caller="02520570", **kw)
            rc, gates, out = cmp(AK, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # the carrier-offset rule (6j) detection (report): the moving BG carrier the camera finds under the player (cam.txt `mvbg`)
        AM = os.path.join(tmp, "a30mvbg")
        write_run(AM, 1, mvbg=(30, 60, 0.0))
        B = os.path.join(tmp, "bmvbg")
        write_run(B, 2, mvbg=(30, 60, 3.0))
        rc, gates, out = cmp(AM, B)
        if gates.get("carrier-bg") != "SKIP" or "on 30 of" not in out or "(00D3), max |B - A| offset 3 " not in out:
            fails.append("moving BG carrier detection: %s\n%s" % (gates.get("carrier-bg"), out))
        else:
            print("ok   moving BG carrier detected on 30 frames, offset 3 (rule note 6j detection, report)")
        # the carrier-offset rule (6j), moving-BG carrier cap: Link and the camera ride a
        # carrier moving 10 a frame; B's carrier (and its rider) is 6.5 off across the motion. Converted carrier: cap
        # 2 + 0.5 x 10 = 7: passes; unconverted: cap 2 (B's rider 4.5 off after it, Link's 6c tolerance 3.9): link-tslack fails (the camera's own drift terms cover 4.5); converted plus a real 6-unit
        # rider error: fails
        AMR = os.path.join(tmp, "a30mvride")
        write_run(AMR, 1, mvbg=(30, 60, 0.0, True, False, 0.0))
        for name, kw, bad in (("Link and camera on a converted moving-BG carrier 6.5 off (rule note 6j, Q7 cap 7)", (30, 60, 6.5, True, True, 0.0), set()),
                              ("the same carrier unconverted (cap pos 2)", (30, 60, 6.5, True, False, 0.0), {"link-tslack"}),
                              ("converted carrier plus a real 6-unit rider error", (30, 60, 6.5, True, True, 6.0), {"link-tslack", "cam-tslack"})):
            B = os.path.join(tmp, "bmvride%d" % (len(bad) * 2 + int(kw[4])))
            write_run(B, 2, mvbg=kw)
            rc, gates, out = cmp(AMR, B)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad:
                fails.append("%s: failed slack gates %s (want %s)\n%s" % (name, sorted(got), sorted(bad), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the RNG-drift plausibility rule (8b) addendum (pair): after the first RNG difference (45) an unconverted
        # RNG-timed source found in one run only is tolerated within the run's last W frames (frames > 88 - W), W = the
        # longest interval of its starts in A's run (the whole remaining window when A plays it once, never when A never
        # plays it); converted sources stay strict
        for name, ka, kb, want in (("RNG-timed source only in B inside the last W = 20 frames (8b addendum)", (10, 30), (10, 30, 75), set()),
                                   ("RNG-timed source only in A inside the last W frames", (10, 30, 75), (10, 30), set()),
                                   ("RNG-timed source only in B before the last W frames", (10, 30), (10, 30, 60), {"sound-rng"}),
                                   ("RNG-timed source A plays once: W = the whole remaining window", (10,), (10, 50), set()),
                                   ("RNG-timed source A never plays", (), (80,), {"sound-rng"})):
            AW = os.path.join(tmp, "a30rngw_%s" % "".join(ch for ch in name if ch.isalnum())[:60])
            write_run(AW, 1, rtse=ka, cse=60, cproc=60)
            B = os.path.join(tmp, "brngw_%s" % "".join(ch for ch in name if ch.isalnum())[:60])
            write_run(B, 2, rngdiv=45, rtse=kb, cse=60, cproc=60)
            rc, gates, out = cmp(AW, B, "--converted", "00A8,01DC")
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        AW = os.path.join(tmp, "a30rngwc")
        write_run(AW, 1, cse=60, cproc=60)
        B = os.path.join(tmp, "brngwc")
        write_run(B, 2, rngdiv=45, cse=85, cproc=60)
        rc, gates, out = cmp(AW, B, "--converted", "00A8,01DC")
        failed = {g for g, st in gates.items() if st == "FAIL"}
        if failed != {"sound"}:
            fails.append("converted sound moved into the last window: failed gates %s (want ['sound'])\n%s" % (sorted(failed), out))
        else:
            print("ok   converted sound moved into the last window stays strict (fails sound as it should)")
        # the aim-boost stop rule (6e): the aim boost's carry when Link stops in procRopeSubject (prope3's 1.44 / 2.78 / 1.42 gap) passes
        # link-tslack only with aim_speed_tol (without it the speed |d| 2.78 > 0.5 fails); B 3 units fast on A's last decelerating frame fails (the cap)
        AA = os.path.join(tmp, "a30aim")
        write_run(AA, 1, aim=(60, 0.0, 0))
        for name, kw, want in (("aim-boost stop transient (rule note 6e, prope3-like)", dict(aim=(60, 0.0, 0)), {"link-motion"}),
                               ("aim-boost stop transient plus a real speed error", dict(aim=(60, 3.0, 3)), {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "baim%d" % len(want))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AA, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s (fails %s as it should)" % (name, ", ".join(sorted(want))))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the turn-factor sampling rule (6g): pgrab1's move start with a turn (measured mNormalSpeed) passes link-tslack; B 2.5 slow on frame F0 + 4
        # on top fails it (tol there 0.5 + 0.73; the +-0.5 frame window of A's ramp alone already absorbs the measured lag,
        # tightest 0.03, so the term only widens the gate by up to 0.8 on these frames, never past A's ramp)
        AG = os.path.join(tmp, "a30acc")
        write_run(AG, 1, accel=(40, 0.0, 0))
        for name, kw, extra, want in (("turn-and-accelerate lag (rule note 6g, pgrab1 166-173)", dict(accel=(40, 0.0, 0)), [], {"link-motion"}),
                                      ("turn-and-accelerate lag plus a real speed error", dict(accel=(40, -2.5, 4)), [], {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "bacc%d" % len(fails) + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AG, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s (fails %s as it should)" % (name, ", ".join(sorted(want))))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the target-facing turn rule (6f): pgrab2's turn to the pot from 6c-drifted positions (1168 units) passes link-tslack with
        # face_angle_tol; fails with the term off (--tol face=0), with B's angle 300 off (B no longer faces the pot), and
        # when A steers away while B keeps facing (the term ends with the facing); both steering away passes
        AT = os.path.join(tmp, "a30face")
        write_run(AT, 1, face=(40, 0, 0))
        AT2 = os.path.join(tmp, "a30face2")
        write_run(AT2, 1, face=(40, 0, 70))
        AT3 = os.path.join(tmp, "a30face3")
        write_run(AT3, 1, face=(40, 0, 0, True))
        for name, a, kw, extra, want in (("target-facing turn from drifted positions (rule note 6f, pgrab2 frame 130)", AT, dict(face=(40, 0, 0)), [], {"link-motion"}),
                                         ("the same with the 6f term off", AT, dict(face=(40, 0, 0)), ["--tol", "face=0"], {"link-motion", "link-tslack"}),
                                         ("target-facing turn plus B's angle 300 off", AT, dict(face=(40, 300, 0)), [], {"link-motion", "link-tslack"}),
                                         ("both runs steer away after facing", AT2, dict(face=(40, 0, 70)), [], {"link-motion"}),
                                         ("A steers away, B keeps facing", AT2, dict(face=(40, 0, -70)), [], {"link-motion", "link-tslack"}),
                                         ("pot lifted with B half a frame ahead, then thrown: the held facing (pgrab4)", AT3, dict(face=(40, 0, 0, True)), [], {"link-motion"}),
                                         ("B's lifted pot 30 off", AT3, dict(face=(40, 0, 0, True, 30.0)), [], {"link-motion", "link-tslack"}),
                                         ("B turns after the release, A does not (the carry ends)", AT3, dict(face=(40, 0, 0, True, 0.0, 75)), [], {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "bface" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(a, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # rule 2 (2026-11-17, keyed pair frame 161): Link half a frame ahead (5 units, no 6c drift) facing a
        # target half a frame behind (4.6): the facing gap (~720 units) is the two leads
        # over the distance and passes link-tslack; fails with the term off, with B's angle 300 off its own facing, with
        # B's target 20 off (outside the window) and with Link a whole frame ahead (outside the window)
        # the lock-on slot takes the actor's eyePos (trace columns 33-35) where the trace has it
        sys.path.insert(0, HERE)
        import compare60 as c60
        # (kddoor): a spec word overlaps a masked 4-byte word only where their bytes meet (its real size)
        ov = c60.word_overlaps
        if any(ov(o, 1, 0xA1C) for o in (0xA19, 0xA1A, 0xA1B)) or not ov(0xA1C, 1, 0xA1C) or not ov(0xA1F, 1, 0xA1C) or \
                not ov(0xA1A, 4, 0xA1C) or ov(0xA20, 2, 0xA1C) or not ov(0xA18, 8, 0xA1C):
            fails.append("word_overlaps: u8 words before a masked word counted as inside it (or a real overlap missed)")
        else:
            print("ok   word_overlaps: by the word's real size (u8 0xA19..0xA1B outside the word at 0xA1C, 0xA1C..0xA1F inside)")
        row = ["0"] * 18 + ["1", "2", "3", "-", "-", "-", "-", "-", "-", "0", "0", "0", "0", "0", "1.0"]
        if c60.targets(row)[0] != [1.0, 2.0, 3.0] or c60.targets(row + ["7", "8", "9"])[0] != [7.0, 8.0, 9.0] or \
                c60.targets(row + ["-", "-", "-"])[0] != [1.0, 2.0, 3.0] or c60.targets(["0"] * 18 + ["-"] * 9 + ["0"] * 6 + ["7", "8", "9"])[0] is not None:
            fails.append("lock-on eye columns 33-35: targets() does not take them as the lock-on slot")
        else:
            print("ok   lock-on slot = eyePos where trace columns 33-35 have it")
        AF = os.path.join(tmp, "a30flead")
        write_run(AF, 1, flead=(60, 0.0, 0.0, 0, 0.0))
        for name, kw, extra, want in (("Link half a frame ahead, target half a frame behind, facing (rule 2, pkk1_2 161)", dict(flead=(60, 0.5, -0.5, 0, 0.0)), [], {"link-motion"}),
                                      ("the same with the 6f term off", dict(flead=(60, 0.5, -0.5, 0, 0.0)), ["--tol", "face=0"], {"link-motion", "link-tslack"}),
                                      ("the leads plus B's angle 300 off its facing", dict(flead=(60, 0.5, -0.5, 300, 0.0)), [], {"link-motion", "link-tslack"}),
                                      ("the leads plus B's target 20 off", dict(flead=(60, 0.5, -0.5, 0, 20.0)), [], {"link-motion", "link-tslack"}),
                                      ("Link a whole frame ahead", dict(flead=(60, 1.0, -0.5, 0, 0.0)), [], {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "bflead" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AF, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s (fails %s as it should)" % (name, ", ".join(sorted(want))))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # 6f chase form (keyed pair frames 157-163): locked on to an actor whose eye moves,
        # each run chases its own direction (setShapeAngleToAtnActor) and lags it by its own chase lag (A ~1 frame's eye
        # motion, B half): passes; fails with the chase form off, with B turning the wrong way, with B's chase stopped and
        # with B a whole frame ahead (outside link-tslack's window)
        AC = os.path.join(tmp, "a30fchase")
        write_run(AC, 1, fchase=(60, 0.0, ""))
        for name, kw, extra, want in (("locked-on chase of a moving eye, B half a frame ahead (6f chase form, pkk1_2 161)", dict(fchase=(60, 0.5, "")), [], {"link-motion"}),
                                      ("the same with the chase form off", dict(fchase=(60, 0.5, "")), ["--tol", "chase=0"], {"link-motion", "link-tslack"}),
                                      ("the chase with B turning the wrong way", dict(fchase=(60, 0.5, "wrong")), [], {"link-motion", "link-tslack"}),
                                      ("the chase with B's chase stopped", dict(fchase=(60, 0.5, "stuck")), [], {"link-motion", "link-tslack"}),
                                      ("the chase with B a whole frame ahead", dict(fchase=(60, 1.0, "")), [], {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "bfchase" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AC, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want - {"link-motion"})) if want - {"link-motion"} else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the process-order read rule (14i) interim (a) (asked on T-03 by pair frame 50):
        # Link locked on to a later-in-line converted enemy whose eye jumps (the jump start): B's Link faces the
        # jump-start eye on frame 60, A's on 61 (the pair's relative eyes: A -30530 / B -29884 / A 41 -30050). With the
        # enemy's own gate report PASS (--target-gate) link-tslack passes; without the report, with the report FAIL,
        # with B 300 off its own eye, and with the term off (face14i=0) it fails; --target-gate FILE:PROC reads only
        # that process's block (the PASS one next to a FAILING other)
        AE = os.path.join(tmp, "a30eye14i")
        write_run(AE, 1)
        post_trace_eye(AE, 1, 60)
        gpass, gfail, gmix = (os.path.join(tmp, x) for x in ("gate14i_pass.txt", "gate14i_fail.txt", "gate14i_mix.txt"))
        open(gpass, "w").write("process 47C5CE68: 216 common frames\n  pos-6c     PASS: tightest frame 181\n")
        open(gfail, "w").write("process 47C5CE68: 216 common frames\n  pos-6c     FAIL: tightest frame 195\n")
        open(gmix, "w").write("process 47C5CE68: 216 common frames\n  pos-6c     PASS: tightest frame 181\n"
                              "process 47C5E81C: 393 common frames\n  pos-6c     FAIL: tightest frame 392\n")
        for name, boff, extra, want in (("lock-on facing a later-in-line enemy's jump-start eye a frame early (14i interim (a), kbkD1_2 50)", 0, ["--target-gate", gpass], {"link-motion"}),
                                        ("the same without the enemy's gate report", 0, [], {"link-motion", "link-tslack"}),
                                        ("the same with the enemy's gate report FAIL", 0, ["--target-gate", gfail], {"link-motion", "link-tslack"}),
                                        ("the same with B 300 off its own eye", 300, ["--target-gate", gpass], {"link-motion", "link-tslack"}),
                                        ("the same with the 14i interim term off", 0, ["--target-gate", gpass, "--tol", "face14i=0"], {"link-motion", "link-tslack"}),
                                        ("the same with the gate report's PASSING process picked (FILE:PROC)", 0, ["--target-gate", gmix + ":47C5CE68"], {"link-motion"}),
                                        ("the same with a gate report whose other process FAILS", 0, ["--target-gate", gmix], {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "beye14i" + str(abs(hash(name))))
            write_run(B, 2)
            post_trace_eye(B, 2, 60, boff)
            rc, gates, out = cmp(AE, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (not (want - {"link-motion"}) and "rule note 14i interim (a) (own-eye facing, later-in-line target): 1 frame(s) (60 +" not in out):
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want - {"link-motion"})) if want - {"link-motion"} else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the animation-phase lead rule (6d(b)): prope7's one-frame lead of the init-replaced animation (R 6: the
        # window covers 3, the term 3) passes link-tslack; the same lead plus a real 0.5-frame error (3) fails; a mapped
        # following animation (k 0.5, rate 2) carries 0.5 x 0.5 x 6 = 1.5 (B 2.5 ahead, window 1: passes); a reset
        # (non-mapped) animation ends the carry (B 1.5 ahead of A's frame 0 at rate 2: residual 1.5 > 1 fails)
        AL = os.path.join(tmp, "a30lead")
        write_run(AL, 1, alead=(40, 6.0, 0.0, None, None))
        ALM = os.path.join(tmp, "a30leadm")
        write_run(ALM, 1, alead=(40, 6.0, 0.0, 60, "map"))
        ALR = os.path.join(tmp, "a30leadr")
        write_run(ALR, 1, alead=(40, 6.0, 0.0, 60, "reset"))
        for name, a, kw, extra, want in (("init-replaced animation one frame ahead (rule note 6d(b), prope7-like)", AL, dict(alead=(40, 6.0, 0.0, None, None)), [], {"link-motion"}),
                                         ("the same with a real 0.5-frame error on top", AL, dict(alead=(40, 6.0, 0.5, None, None)), [], {"link-motion", "link-tslack"}),
                                         ("the lead carried into a phase-mapped animation", ALM, dict(alead=(40, 6.0, 0.0, 60, "map")), [], {"link-motion"}),
                                         ("a non-mapped animation change ends the carry", ALR, dict(alead=(40, 6.0, 0.0, 60, "reset")), [], {"link-motion", "link-tslack"})):
            B = os.path.join(tmp, "blead" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(a, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the ship-drift rule (6h) (hold form): the ship's drift (route: speed ramp 0 -> 40 over frames 10-19,
        # steady, turn 300 units per frame from frame 40). ramp x (|sum dv_A| + |sum |v_A| dheading_A|), both sums signed,
        # never reset: a turn passes, a turn plus a real 30-unit error fails, a straight run after the turn passes (the
        # offset the turn left is kept, as measured in ps3: 32.9 units on the straight run 250-300) and fails with a real
        # 30-unit error on it; a turn and the opposite turn cancel (the offset returns, so does the tolerance: bounded)
        # and a real 30-unit error after them fails
        sys.path.insert(0, HERE)
        import compare60 as c60
        AH = os.path.join(tmp, "a30ship")
        write_run(AH, 1, ship=(40, FRAMES, 300, None))
        AH2 = os.path.join(tmp, "a30ship2")
        write_run(AH2, 1, ship=(40, 60, 300, None))
        AH3 = os.path.join(tmp, "a30ship3")
        write_run(AH3, 1, ship=(40, 55, 300, None, 70))
        bnames = {}
        for name, a, kw, want in (("ship turn to the end of the route (rule note 6h)", AH, dict(ship=(40, FRAMES, 300, None)), set()),
                                  ("ship turn plus a real 30-unit error", AH, dict(ship=(40, FRAMES, 300, (70, 30.0))), {"ship"}),
                                  # x 2: after 30 frames of continuous turning (82 deg) the tolerance is 29.8, so a 30-unit
                                  # error against the drift's direction hides there; 45 units fail
                                  ("ship turn plus a real 45-unit error", AH, dict(ship=(40, FRAMES, 300, (70, 45.0))), {"ship"}),
                                  ("ship straight run after a turn (the turn's offset held)", AH2, dict(ship=(40, 60, 300, None)), set()),
                                  ("ship straight run after a turn plus a real 30-unit error", AH2, dict(ship=(40, 60, 300, (80, 30.0))), {"ship"}),
                                  ("ship turn and opposite turn (cancel)", AH3, dict(ship=(40, 55, 300, None, 70)), set()),
                                  ("ship turn and opposite turn plus a real 30-unit error", AH3, dict(ship=(40, 55, 300, (80, 30.0), 70)), {"ship"})):
            B = os.path.join(tmp, "bship%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, **kw)
            # the default (heading term x 2: arc + tiller lag) and the arc alone (x 1): the
            # real errors fail in both; a 30-unit error inside a continuous turn hides at x 2 (scalar bound, see above)
            for extra in ([], ["--tol", "ship_turn_k=1"]):
                if not extra and "30-unit" in name and "ship turn plus" in name:
                    continue
                rc, gates, out = cmp(a, B, *extra)
                failed = {g for g, st in gates.items() if st == "FAIL"}
                tag = name + (" [ship_turn_k 1]" if extra else "")
                if failed != want or gates.get("ship") not in ("PASS", "FAIL"):
                    fails.append("%s: failed gates %s (want %s)\n%s" % (tag, sorted(failed), sorted(want), out))
                else:
                    print("ok   %s%s" % (tag, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
                if os.environ.get("C60_DEBUG"):
                    print(out)
        # the early ship-decision rule (6i): the sail-up decision shift (B one record frame early: mode change and
        # speed step 0 -> 40 at frame 30, turn 40-60). With the shift detected: passes (speed on the switch frames
        # plausibility only, xz + (1 - dt_B) x max |v_A| = 20 after it); the same plus a real 30-unit error after the
        # turn fails; no shift (both switch on the same frame) but B 20 units ahead (a real error the size of the
        # term): no term, fails
        AJ = os.path.join(tmp, "a30shipj")
        write_run(AJ, 1, ship=(40, 60, 300, None, None, (30, False)))
        AJF = os.path.join(tmp, "a30shipjf")
        write_run(AJF, 1, ship=(40, 60, 300, None, None, (30, False), None, (62, 84, 0.0, 0.0, False)))
        for name, kw, want, sh in (("ship sail-up decision shift (rule note 6i)", dict(ship=(40, 60, 300, None, None, (30, True))), set(), "B 20 / A 21"),
                                   ("ship sail-up decision shift plus a real 30-unit error after the turn", dict(ship=(40, 60, 300, (70, 30.0, "along"), None, (30, True))), {"ship"}, "B 20 / A 21"),
                                   ("ship no decision shift, B 20 units ahead (no 6i term)", dict(ship=(40, 60, 300, (31, 20.0, "along"), None, (30, False))), {"ship"}, "none"),
                                   # rule (c): y report-only plus plausibility: 30 units above A's float range fails
                                   ("ship y off the sea band (plausibility, rule (c))", dict(ship=(40, 60, 300, (50, 30.0, "y"), None, (30, False))), {"ship"}, "none"),
                                   # (ship y in flight, pj2): B takes off 1.8 higher (the float gap)
                                   # and flies the same arc: its apex is outside A's absolute float range, its height over
                                   # its own take-off is not: passes; + a real 10-unit error in the flight fails; a jump
                                   # only A makes fails
                                   ("ship jump, B's take-off 1.8 higher (y in flight relative to the take-off)", dict(ship=(40, 60, 300, None, None, (30, True), None, (62, 84, 1.8, 0.0, False))), set(), "B 20 / A 21"),
                                   ("ship jump plus a real 10-unit height error in flight", dict(ship=(40, 60, 300, None, None, (30, True), None, (62, 84, 1.8, 10.0, False))), {"ship"}, "B 20 / A 21"),
                                   ("ship jump only in A", dict(ship=(40, 60, 300, None, None, (30, True), None, (62, 84, 1.8, 0.0, True))), {"ship"}, "B 20 / A 21")):
            B = os.path.join(tmp, "bship%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AJF if len(kw["ship"]) > 7 else AJ, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or gates.get("ship") not in ("PASS", "FAIL") or ("shifts (B earlier) " + ("mode " if sh != "none" else "") + sh) not in out:
                fails.append("%s: failed gates %s (want %s, shifts %s)\n%s" % (name, sorted(failed), sorted(want), sh, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the ship-drift rule (6h) (per-episode speed part) and 6i(i) (pair): a start (B 10 behind
        # after the frame's speed step) and a stop whose sail-down B takes on the FIRST step of A's record frame (the
        # coast-down half a frame early: B another 10.2 behind). Passes: 6h's speed part sums |net dv| per monotone
        # episode (start 40 + coast 39.65: 2 + 19.9; the signed sum would give ~0), 6i(i) treats the same-frame
        # half-step decision as a shift of 0.5 (speed plausibility at the switch, xz + 0.5 x max |v_A| while the coast
        # runs, speed + 0.5 x |dv_A|); the same plus a real 30-unit error from frame 50 fails (once the coast's term has
        # ended); the same stop taken on B's last step (no shift) passes on 6h alone
        AS = os.path.join(tmp, "a30ships")
        write_run(AS, 1, ship=(FRAMES, FRAMES, 0, None, None, None, (10, 40, True)))
        for name, kw, want, sh in (("ship start + stop, the sail-down on B's first step (rule notes 6h episodes, 6i(i))", dict(ship=(FRAMES, FRAMES, 0, None, None, None, (10, 40, True))), set(), "mode B 40 / A 40 (step shift 0.5)"),
                                   ("ship start + stop plus a real 30-unit error", dict(ship=(FRAMES, FRAMES, 0, (50, 30.0), None, None, (10, 40, True))), {"ship"}, "mode B 40 / A 40 (step shift 0.5)"),
                                   ("ship start + stop, the sail-down on B's last step (no shift)", dict(ship=(FRAMES, FRAMES, 0, None, None, None, (10, 40, False))), set(), "none")):
            B = os.path.join(tmp, "bship%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AS, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or gates.get("ship") not in ("PASS", "FAIL") or ("shifts (B earlier) " + sh) not in out:
                fails.append("%s: failed gates %s (want %s, shifts %s)\n%s" % (name, sorted(failed), sorted(want), sh, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the early ship-decision rule (6i(iii)) (pair): a DEMO_MOVE arrival (stop on the arrival radius, a
        # decision on position, ship mode 7). the pair's numbers: B 5.65 behind at |v_A| 2 on the frame before A's stop -> k
        # <= 3; B stops 63 / A 60, rest |xz| 0.35: passes; B stopping first (6h's drift puts it 9 ahead: k <= 5, B 55 /
        # A 60) passes; the same arrival in mode 1 (no position decision), with 6i(iii) off, with B's rest 3 off, and with
        # B 5 frames late (beyond k 3) fail
        def arr(lag, delay, rerr, mode):
            return (FRAMES, FRAMES, 0, None, None, None, None, None, None, None, None, None, None, (lag, delay, rerr, mode))
        AAR = {m: os.path.join(tmp, "a30shiparr%d" % m) for m in (7, 1)}
        for m, d in AAR.items():
            write_run(d, 1, ship=arr(0.0, 0, 0.0, m))
        for name, kw, tol, want, sh in (("ship DEMO_MOVE arrival 3 frames late (rule note 6i(iii), fst8 numbers)", (14.65, 1, 0.0, 7), (), set(), "stop B 63 / A 60 (k 3 <= ceil(|xz| 5.65 / |v_A| 2) = 3; rest frame 63 |xz| 0.35)"),
                                        ("ship DEMO_MOVE arrival, B stops first (6i(iii), measured before B's stop)", (0.0, 0, 0.0, 7), (), set(), "stop B 55 / A 60 (k 5"),
                                        ("ship arrival 3 frames late outside a position-decision mode", (14.65, 1, 0.0, 1), (), {"ship"}, "none"),
                                        ("ship DEMO_MOVE arrival 3 frames late with 6i(iii) off", (14.65, 1, 0.0, 7), ("--tol", "ship_pos_shift=0"), {"ship"}, "none"),
                                        ("ship DEMO_MOVE arrival 3 frames late, B's rest 3 units off", (14.65, 1, 3.0, 7), (), {"ship"}, "stop B 63 / A 60"),
                                        ("ship DEMO_MOVE arrival 5 frames late (beyond k 3)", (14.65, 3, 0.0, 7), (), {"ship"}, "none"),
                                        # the same frame in both runs (k 0, the pair's 451): no shift, 6i(iii) stays out; B's
                                        # rest 3 off is judged by the xz gate's pos + 6h drift, not by pos alone
                                        ("ship DEMO_MOVE arrival on A's frame, B's rest 3 off inside 6h (6i(iii) not applied)", (10.0, 0, 3.0, 7), (), set(), "none")):
            B = os.path.join(tmp, "bshiparr%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=arr(*kw))
            rc, gates, out = cmp(AAR[kw[3]], B, *tol)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or ("6i(iii) position decisions " + sh) not in out:
                fails.append("%s: failed gates %s (want %s, 6i(iii) %s)\n%s" % (name, sorted(failed), sorted(want), sh, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the early ship-decision rule (6i(iii)), stricter pairing: a B switch on the same frame as A's is that
        # switch's pair and both are consumed, so (a) A's stop on 60 never pairs with B's later stop on 62 (B stops 60,
        # starts 61, stops 62; 5 behind at |v_A| 2: k <= 3 would have let it), and (b) B's stop on 60 never pairs with
        # A's earlier stop on 58; the previous form (`continue` past the same-frame B) paired both
        sys.path.insert(0, HERE)
        import compare60 as c60

        def pos_rows(stops, lag):
            rows, z = {}, 0.0
            for f in range(40, 81):
                v = 0.0 if stops(f) else 2.0
                rows[f] = {"mode": 7, "speedF": v, "pos": (0.0, 0.0, z - lag)}
                z += v
            return rows
        for name, sa_, sb_, want in (
                ("6i(iii) stricter pairing: A's same-frame stop never takes B's later stop",
                 pos_rows(lambda f: f >= 60, 0.0), pos_rows(lambda f: f == 60 or f >= 62, 5.0), []),
                ("6i(iii) stricter pairing: B's same-frame stop never takes A's earlier stop",
                 pos_rows(lambda f: f == 58 or f >= 60, 0.0), pos_rows(lambda f: f >= 60, 5.0), [])):
            got = [(fb, fa) for fb, fa, *_ in c60.ship_pos_shifts(sa_, sb_, [])[0]]
            if got != want:
                fails.append("%s: pairs %s, want %s" % (name, got, want))
            else:
                print("ok   %s" % name)
        # control: B's only stop 2 frames after A's still pairs (k 2 <= 3)
        got = [(fb, fa) for fb, fa, *_ in c60.ship_pos_shifts(pos_rows(lambda f: f >= 60, 0.0), pos_rows(lambda f: f >= 62, 5.0), [])[0]]
        if got != [(62, 60)]:
            fails.append("6i(iii) stricter pairing control (B 62 / A 60 alone): pairs %s, want [(62, 60)]" % got)
        else:
            print("ok   6i(iii) stricter pairing control: a lone B stop 2 frames late still pairs")
        # the early ship-decision rule (6i) addendum (pairs): the sail's wind power reaches 0 inside
        # frame 30 (B switches to the 0.1 decline on the frame's second step: 0.95 higher, kept until the speeds converge
        # after the chase back from frame 50) passes; the same plus a real 1.5 speed error from frame 80 (after the
        # window closed) fails; the crossing with the addendum off (ship_branch_step 0) fails
        ABR = os.path.join(tmp, "a30shipbr")
        write_run(ABR, 1, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, None, (30, 50, None)))
        for name, kw, tol, want in (("ship sail branch change inside a frame (rule note 6i addendum)", (30, 50, None), [], set()),
                                    ("ship sail branch change plus a real 1.5 speed error outside the window", (30, 50, (80, 1.5)), [], {"ship"}),
                                    ("ship sail branch change with the addendum off", (30, 50, None), ["--tol", "ship_branch_step=0"], {"ship"})):
            B = os.path.join(tmp, "bshipbr%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, None, kw))
            rc, gates, out = cmp(ABR, B, *tol) if tol else cmp(ABR, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (not want and "branch changes (sail: wind power 0 inside a frame) frames 3" not in out):
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # rule (c) addendum (pair) with the derived slope bound
        # (|mWaterY_B - mWaterY_A| <= s_A x |xz B - A| + pos where a 6i term displaces B's ship): B's ship
        # displaced by the 6i addendum's branch change on a sea sloping 0.3 a unit along the route (s_A measured 0.3) sits
        # higher by the slope x its lead: passes; the same + a 10-unit water jump from frame 40 that the xz distance does
        # not explain fails; on A's flat sea (s_A 0) the 10-unit crest fails (the old skip passed it, `--tol sea_slope=0`
        # still does); a float offset 5 off fails; the crest without a 6i term (B on A's path) fails the band
        ABS = os.path.join(tmp, "a30shipbrs")
        write_run(ABS, 1, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, None, (30, 50, None), None, 0.3))
        for name, kw, want, tol in (("ship displaced by a 6i term on a sloping sea (rule (c) slope bound)", ("slope", None), set(), ()),
                                    ("ship displaced by a 6i term on a sloping sea + a 10-unit water jump", ("slope", (40, 10.0, 0.0)), {"ship"}, ()),
                                    ("ship displaced by a 6i term on a crest of a flat sea (s_A 0)", (None, (40, 10.0, 0.0)), {"ship"}, ()),
                                    ("ship displaced by a 6i term on a crest, slope bound off (sea_slope=0: old skip)", (None, (40, 10.0, 0.0)), set(), ("--tol", "sea_slope=0")),
                                    ("ship displaced by a 6i term on a wave crest with a wrong float offset", (None, (40, 10.0, 5.0)), {"ship"}, ("--tol", "sea_slope=0")),
                                    ("ship on a wave crest outside A's band without a 6i term", ("plain", (40, 10.0, 0.0)), {"ship"}, ())):
            B = os.path.join(tmp, "bshipcr%d" % len(bnames))
            bnames[name] = B
            if kw[0] == "plain":
                AA = os.path.join(tmp, "a30shipcrp")
                if not os.path.exists(AA):
                    write_run(AA, 1, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, None, None))
                write_run(B, 2, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, None, None, kw[1]))
            elif kw[0] == "slope":
                AA = ABS
                write_run(B, 2, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, None, (30, 50, None), kw[1], 0.3))
            else:
                AA = ABR
                write_run(B, 2, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, None, (30, 50, None), kw[1]))
            rc, gates, out = cmp(AA, B, *tol) if tol else cmp(AA, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (name.endswith("bound)") and ("replaced by the slope bound on 0 " in out or "s_A max 0.3" not in out)):
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the animation-phase lead rule (6d) addendum: a running animation's rate changes. the pair's
        # measured wait animation (N 4: the ramp 265-285, the drop 2.375 -> 1.1 at 286, B 1.05 past the window at 293)
        # passes link-tslack with the rate-step term and fails without it (--tol ratestep=0); a rate jump 0 -> 5 at N 4
        # (B 3.75 ahead, window 2.5) passes, fails without the term, and fails with a real 0.5-frame error on top; at N 2
        # (B 2.5 ahead) passes
        RA = os.path.join(tmp, "a30rprun")
        write_run(RA, 1, rstep=("prun",))
        RJ = os.path.join(tmp, "a30rjump")
        write_run(RJ, 1, rstep=("jump", 40, 0.0, 5.0, 0.0))
        for name, a, n, kw, extra, want in (("prun4a's running-animation rate steps (rule note 6d addendum)", RA, 4, dict(rstep=("prun",)), [], {"link-motion"}),
                                            ("the same without the rate-step term", RA, 4, dict(rstep=("prun",)), ["--tol", "ratestep=0"], {"link-motion", "link-tslack"}),
                                            ("a rate jump 0 -> 5 of a running animation at N 4", RJ, 4, dict(rstep=("jump", 40, 0.0, 5.0, 0.0)), [], {"link-motion"}),
                                            ("the same without the rate-step term", RJ, 4, dict(rstep=("jump", 40, 0.0, 5.0, 0.0)), ["--tol", "ratestep=0"], {"link-motion", "link-tslack"}),
                                            ("the same with a real 0.5-frame error on top", RJ, 4, dict(rstep=("jump", 40, 0.0, 5.0, 0.5)), [], {"link-motion", "link-tslack"}),
                                            ("a rate jump 0 -> 5 of a running animation at N 2", RJ, 2, dict(rstep=("jump", 40, 0.0, 5.0, 0.0)), [], {"link-motion"})):
            B = os.path.join(tmp, "brstep" + str(abs(hash(name + a + str(kw) + str(extra) + str(n)))))
            write_run(B, n, **kw)
            rc, gates, out = cmp(a, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the early ship-decision rule (6i(ii)) (crane pair): the crane-up switch from the
        # unconverted event system lands on B's last step (B half a frame later, its rope 2 longer); B's lift-up 3
        # frames late passes (derived D = 2 segments x 1 frame + se_slack 1; the old procedure's work sound on B's step 0
        # of the switch frame is that share); 5 frames late fails (beyond the derived bound); a switch on B's FIRST step
        # (no B-later shift, the same rope) with the lift-up 3 frames late fails.:
        # beyond the derived bound after a B-later shift the rope-timed lift-up is plausibility (reported, not
        # failed) until with --tol crane_rest=0 5 frames late fails
        ACR = os.path.join(tmp, "a30crane")
        write_run(ACR, 1, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, (40, False, 0)))
        for name, kw, want, sh, ex in (("ship crane-up on B's last step, lift-up 3 frames late (rule note 6i(ii), derived D 2)", (40, True, 3), set(), "mode B 40 / A 40 (shift 0.5)", []),
                                   ("ship crane-up on B's last step, lift-up 5 frames late (beyond the derived bound: rule (b) plausibility)", (40, True, 5), set(), "mode B 40 / A 40 (shift 0.5)", []),
                                   ("ship crane-up on B's last step, lift-up 5 frames late, --tol crane_rest=0", (40, True, 5), {"sound"}, "mode B 40 / A 40 (shift 0.5)", ["--tol", "crane_rest=0"]),
                                   ("ship crane-up on B's first step (no B-later shift), lift-up 3 frames late", (40, False, 3), {"sound"}, "none", [])):
            B = os.path.join(tmp, "bcrane%d" % kw[2] + ("l" if kw[1] else "e"))
            if not os.path.exists(B):
                write_run(B, 2, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, kw))
            rc, gates, out = cmp(ACR, B, *ex)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or gates.get("ship") != "PASS" or ("shifts (B later) " + sh) not in out:
                fails.append("%s: failed gates %s (want %s, B-later shifts %s)\n%s" % (name, sorted(failed), sorted(want), sh, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the step-resolution threshold rule (6l) + addenda: / pfan5n4-like threshold events, each run at
        # its own step: the opening kick one frame early in B (B's steps cross the animation frame's > 2.0 first) and the
        # gravity switch `speed.y < 2.5` one frame early ((a): a slope change; at N 4 one frame past the kick's carried
        # lead, the pair's 107 / 109) pass link-tslack and cam-tslack at N 2 and N 4; at N 4 B's speed curve after the kick
        # leads by 0.75 frame: fails without the derived speed form (--tol event_dv=0), and without the slope event
        # (--tol event_slope=0); --tol event=0 fails; B two frames early (the 2-frame shift) fails; one frame late fails;
        # a real 20-unit error after the shared change (converged) fails. (b): a position-driven animation switch a frame
        # late or early while the offset is carried passes; two frames late, a different new animation, or an extra
        # switch fail. (c): the offset unconverged at the shared change (the leaf close -> a fall from each run's own
        # height, B pushed 0.6 / frame in z): plausibility to the landing, strict once the gap is back (a 20-unit error
        # in the fall passes, one after the landing fails); the gap never back: reported, not failed
        AK = os.path.join(tmp, "a30kick")
        write_run(AK, 1, kick=(60, 0, 0.0, None))
        AKG = os.path.join(tmp, "a30kickg")
        write_run(AKG, 1, kick=(60, 0, 0.0, None, "gust"))
        AKF = os.path.join(tmp, "a30kickf")
        write_run(AKF, 1, kick=(60, 0, 0.0, None, "fall"))
        AKI = os.path.join(tmp, "a30kicki")
        write_run(AKI, 1, kick=(60, 0, 0.0, None, "init"))
        for name, a, n, kw, extra, want, note in (
                ("event one frame early (rule note 6l, pfan4-like)", AK, 2, (60, 0, 0.0, None), [], {"link-motion"}, "speed slope 2 at frame 66 in A, frame 65 in B (one frame early)"),
                ("event one frame early at N 4 (pfan5n4-like: the gravity switch past the carried lead)", AK, 4, (60, 0, 0.0, None), [], {"link-motion"}, "frame 64 in B (one frame early past a carried lead of 1)"),
                ("the same at N 4 without the derived speed form (--tol event_dv=0)", AK, 4, (60, 0, 0.0, None), ["--tol", "event_dv=0"], {"link-motion", "link-tslack"}, None),
                ("the same at N 4 without the slope event (--tol event_slope=0)", AK, 4, (60, 0, 0.0, None), ["--tol", "event_slope=0"], {"link-motion", "link-tslack"}, None),
                ("the same without the term (--tol event=0)", AK, 2, (60, 0, 0.0, None), ["--tol", "event=0"], {"link-motion", "link-tslack"}, None),
                ("event two frames early (a 2-frame shift)", AK, 2, (60, -1, 0.0, None), [], {"link-motion", "link-tslack"}, "more than one frame early"),
                ("event one frame late", AK, 2, (60, 2, 0.0, None), [], {"link-procs", "link-motion", "link-tslack"}, "(late)"),
                ("one frame early plus a real 20-unit error after convergence", AK, 2, (60, 0, 20.0, 85), [], {"link-motion", "link-tslack"}, None),
                ("init: a procedure change one frame early whose init sets the speed (6l addenda, psword2-like)", AKI, 2, (60, 0, 0.0, None, "init"), [], {"link-motion"}, "speed init 13.46 at frame 60 in A, frame 59 in B (one frame early"),
                ("init: the same at N 4", AKI, 4, (60, 0, 0.0, None, "init"), [], {"link-motion"}, "speed init 13.46 at frame 60 in A"),
                ("init: the same without the term (--tol event=0)", AKI, 2, (60, 0, 0.0, None, "init"), ["--tol", "event=0"], {"link-motion", "link-tslack"}, None),
                ("init: the procedure change two frames early", AKI, 2, (60, -1, 0.0, None, "init"), [], {"link-procs", "link-motion", "link-tslack"}, "more than one frame early"),
                ("(b) animation switch a frame late while carried (pfan4 149 / 165)", AKG, 2, (60, 0, 0.0, None, "gust"), [], {"link-motion"}, "2 a frame apart (A/B 66/67, 72/73)"),
                ("(b) animation switch a frame early while carried, N 4", AKG, 4, (60, 0, 0.0, None, "gustearly"), [], {"link-motion"}, "2 a frame apart (A/B 66/65, 72/71)"),
                ("(b) animation switch two frames late", AKG, 2, (60, 0, 0.0, None, "gust2"), [], {"link-motion", "link-tslack"}, "differ"),
                ("(b) an animation sequence that differs (another new animation)", AKG, 2, (60, 0, 0.0, None, "gustseq"), [], {"link-motion", "link-tslack"}, None),
                ("(b) an animation sequence that differs (an extra switch)", AKG, 2, (60, 0, 0.0, None, "gustextra"), [], {"link-motion", "link-tslack"}, "differ"),
                ("(c) unconverged at the shared change: plausibility to the landing", AKF, 2, (60, 0, 0.0, None, "fall"), [], {"link-motion"}, "strict from there"),
                ("(c) the same at N 4", AKF, 4, (60, 0, 0.0, None, "fall"), [], {"link-motion"}, "strict from there"),
                ("(c) a 20-unit error in the fall (plausibility) passes", AKF, 2, (60, 0, 20.0, 82, "fall"), [], {"link-motion"}, None),
                ("(c) a real 20-unit error after the landing (converged) fails", AKF, 2, (60, 0, 20.0, 88, "fall"), [], {"link-motion", "link-tslack"}, None),
                ("(c) the gap never back after the landing: reported, not failed", AKF, 2, (60, 0, 0.0, None, "fallstay"), [], {"link-motion"}, "a reported, not failed, divergence")):
            B = os.path.join(tmp, "bkick" + str(abs(hash(name))))
            write_run(B, n, kick=kw)
            rc, gates, out = cmp(a, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (note is not None and note not in out):
                fails.append("%s: failed gates %s (want %s), note %r\n%s" % (name, sorted(failed), sorted(want), note, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the crane / salvage window (from the first crane-up frame - proc_slack
        # to the end) is plausibility only until: a follow-on sound (procCraneUp's swing 0x381E) 3 frames late passes
        # within se_slack + D (2); 5 frames late fails; with --tol crane_plaus=0 (strict, after) 3 late fails; a
        # 40-unit Link position error inside the window is no per-frame check (passes); B's Link stopped from frame 75
        # while A's still moves fails crane (stuck)
        def full_step(n, f):
            return 1000 - 5 * n + (f + 5) * n + n - 1
        def add_se(d, n, f, sid="0000381E"):
            open(os.path.join(d, "se.txt"), "a").write("%d 1 0 %s 0247FC0C\n" % (full_step(n, f), sid))
        def freeze_link(d, n, f0):
            out, x0 = [], None
            for l in open(os.path.join(d, "trace.txt")).read().split("\n"):
                c = l.split()
                if len(c) > 8 and not l.startswith(" ") and c[1].isdigit() and (int(c[1]) - (1000 - 5 * n)) // n - 5 >= f0:
                    x0 = c[6] if x0 is None else x0
                    c[6] = x0
                    l = " ".join(c)
                out.append(l)
            open(os.path.join(d, "trace.txt"), "w").write("\n".join(out))
        ACQ = os.path.join(tmp, "a30craneq8")
        write_run(ACQ, 1, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, (40, False, 0)))
        add_se(ACQ, 1, 60)
        for qi, (name, lateby, extra, kw, frz, want) in enumerate((
                ("Q8: crane window, swing sound 3 frames late (se_slack + D 2)", 3, [], {}, None, set()),
                ("Q8: crane window, swing sound 5 frames late", 5, [], {}, None, {"sound"}),
                ("Q8: --tol crane_plaus=0 (strict after M5), swing sound 3 frames late", 3, ["--tol", "crane_plaus=0"], {}, None, {"sound"}),
                ("Q8: crane window, Link 40 units off at one frame (no per-frame check)", 0, [], dict(pos_err=40.0), None, set()),
                ("Q8: crane window, B's Link stopped from frame 75 while A's moves (stuck)", 0, [], {}, 75, {"crane"}))):
            B = os.path.join(tmp, "bcraneq8_%d" % qi)
            write_run(B, 2, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, (40, True, 2)), **kw)
            add_se(B, 2, 60 + lateby)
            if frz is not None:
                freeze_link(B, 2, frz)
            rc, gates, out = cmp(ACQ, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL" and g not in ("link-motion", "camera")}
            want_crane = "SKIP" if extra else "FAIL" if "crane" in want else "PASS"
            if failed != want or gates.get("crane") != want_crane:
                fails.append("%s: failed gates %s (want %s), crane %s\n%s" % (name, sorted(failed), sorted(want), gates.get("crane"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # 6l addendum (g) (257/258 + 261/262): a position-driven speed step
        # (below event_jump) one frame early in B while B's position lead (13.3 = 0.44 frame) is inside the time slack
        # passes link-tslack at N 2 and N 4; without the term (--tol event_pos=0) it fails; B lagging (late steps) fails;
        # B's lead outside the slack (20 = 0.67 frame) fails; a real 1-unit speed error of B after convergence fails; no
        # lead (the step-resolution rounding alone, 6l's own class): one frame early, passes
        AP = os.path.join(tmp, "a30pstep")
        write_run(AP, 1, pstep=(0.0, 0.0))
        for name, n, kw, extra, want, note in (
                ("position-driven speed step one frame early (6l addendum (g), psword2g-like)", 2, (13.3, 0.0), [], {"link-motion"}, "speed pos 2.55 at frame 65 in A, frame 64 in B (one frame early)"),
                ("the same at N 4 (lead 6 = 0.2 frame)", 4, (6.0, 0.0), [], {"link-motion"}, "speed pos 1.1 at frame 61 in A, frame 60 in B (one frame early)"),
                ("the same without the term (--tol event_pos=0)", 2, (13.3, 0.0), ["--tol", "event_pos=0"], {"link-motion", "link-tslack"}, None),
                ("B lagging: the steps a frame late", 2, (-13.3, 0.0), [], {"link-motion", "link-tslack"}, None),
                ("B's lead outside the time slack (0.67 frame)", 2, (20.0, 0.0), [], {"link-motion", "link-tslack"}, None),
                ("one frame early plus a real 1-unit speed error after convergence", 2, (13.3, 1.0), [], {"link-motion", "link-tslack"}, None),
                ("no lead: the step-resolution rounding alone, one frame early", 2, (0.0, 0.0), [], {"link-motion"}, "frame 60 in B (one frame early)")):
            B = os.path.join(tmp, "bpstep" + str(abs(hash(name))))
            write_run(B, n, pstep=kw)
            rc, gates, out = cmp(AP, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (note is not None and note not in out):
                fails.append("%s: failed gates %s (want %s), note %r\n%s" % (name, sorted(failed), sorted(want), note, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # rule addendum (2026-11-16, pcr10: A swings 96/109/125/141, B 96/109/124/139/154): a
        # periodic sound inside the crane window may differ in count by at most one, the instances otherwise keeping the
        # same spacing within se_slack + D; pcr10-like (one more B swing at the end, B's spacing a frame shorter) passes;
        # two more fails; one more out of the spacing fails
        for qi, (name, fa_, fb_, want) in enumerate((
                ("Q8 addendum: crane window, periodic swing B one more (pcr10-like)", (44, 54, 64, 74), (44, 53, 63, 73, 83), set()),
                ("Q8 addendum: crane window, periodic swing B two more", (44, 54, 64), (44, 54, 64, 74, 84), {"sound"}),
                ("Q8 addendum: crane window, periodic swing B one more out of the spacing", (44, 54, 64, 74), (44, 53, 63, 73, 78), {"sound"}))):
            A = os.path.join(tmp, "a30cranep_%d" % qi)
            B = os.path.join(tmp, "bcranep_%d" % qi)
            write_run(A, 1, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, (40, False, 0)))
            write_run(B, 2, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, (40, True, 2)))
            for f in fa_:
                add_se(A, 1, f)
            for f in fb_:
                add_se(B, 2, f)
            rc, gates, out = cmp(A, B)
            failed = {g for g, st in gates.items() if st == "FAIL" and g not in ("link-motion", "camera")}
            if failed != want or gates.get("crane") != "PASS":
                fails.append("%s: failed gates %s (want %s), crane %s\n%s" % (name, sorted(failed), sorted(want), gates.get("crane"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # 6l addendum (g) on the camera (frame 214): a camera type change one
        # frame early in B (the centre then a frame ahead on its chase of the new target, 30 units) passes cam-tslack at N 2
        # and N 4 (half a frame); without the term (--tol cam_type=0) it fails; two frames early fails; one frame early plus
        # a real centre error fails: 25 units on the change frame (more than the frame's 22.5-unit move) or 4 units on the
        # frame after it (outside the term); a change at the same frame passes
        AT = os.path.join(tmp, "a30ctype")
        write_run(AT, 1, ctype=(60, 30.0, 0.0, 0.0))
        for name, n, kw, extra, want, note in (
                ("camera type change one frame early (6l addendum (g) on the camera)", 2, (60, 30.0, 1.0, 0.0), [], {"camera"}, "centre term on 2 frame(s) (frame 59 +22.5, frame 60 +22.5)"),
                ("the same at N 4, half a frame early", 4, (60, 30.0, 0.5, 0.0), [], {"camera"}, "centre term on 2 frame(s)"),
                ("the same frame in both runs", 2, (60, 30.0, 0.0, 0.0), [], set(), None),
                ("one frame early without the term (--tol cam_type=0)", 2, (60, 30.0, 1.0, 0.0), ["--tol", "cam_type=0"], {"camera", "cam-tslack"}, None),
                ("two frames early", 2, (60, 30.0, 2.0, 0.0), [], {"camera", "cam-tslack"}, None),
                ("one frame early plus a real 25-unit centre error on the change frame", 2, (60, 30.0, 1.0, 25.0), [], {"camera", "cam-tslack"}, None),
                ("one frame early plus a real 4-unit centre error on the next frame", 2, (60, 30.0, 1.0, 4.0, 1), [], {"camera", "cam-tslack"}, None)):
            B = os.path.join(tmp, "bctype" + str(abs(hash(name))))
            write_run(B, n, ctype=kw)
            rc, gates, out = cmp(AT, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (note is not None and note not in out):
                fails.append("%s: failed gates %s (want %s), note %r\n%s" % (name, sorted(failed), sorted(want), note, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # 6l addendum (g) carry (pair 131): B's type change in the same frame but half
        # a frame later (Link crosses the floor in the frame's second step) leaves the new style's centre half a frame of
        # its start behind; the change-frame term carries, decaying with the style's centre follow (style 2 = FixdPos
        # fixedPositionCamera: 1 - min(0.75, 0.25) = 0.75 per frame), so cam-tslack passes; without the carry
        # (--tol cam_type_carry=0) it fails; a real 9-unit centre error on F + 4 (inside the carry, over its decayed term)
        # and a 4-unit error on F + 10 (after the carry ended) fail; the same lag in a style without centre ratios (4, the
        # event camera) gets no carry and fails
        ACC = os.path.join(tmp, "a30ccarry")
        write_run(ACC, 1, ccarry=(60, 40.0, 0.0, 0.75, 2))
        ACC4 = os.path.join(tmp, "a30ccarry4")
        write_run(ACC4, 1, ccarry=(60, 40.0, 0.0, 0.75, 4))
        ACC56 = os.path.join(tmp, "a30ccarry56")
        write_run(ACC56, 1, ccarry=(60, 40.0, 0.0, 0.75, 56))
        for name, a, kw, extra, want, note in (
                ("type change half a frame later in the same frame (6l addendum (g) carry)", ACC, (60, 40.0, 0.5, 0.75, 2), [], {"camera"}, "frame 61 +12.4"),
                ("the same without the carry (--tol cam_type_carry=0, cchase=0: in style 2 the 6c Link-following centre chase term also covers this y chase)", ACC, (60, 40.0, 0.5, 0.75, 2), ["--tol", "cam_type_carry=0", "--tol", "cchase=0"], {"camera", "cam-tslack"}, None),
                # the carry alone (2026-11-18): style 56 has the carry's centre decay (0.75) but no 6c centre-chase
                # ratios, so the carry is the only term: passes with it, fails with --tol cam_type_carry=0 alone
                ("the same lag in style 56 (carry, no 6c centre-chase ratios)", ACC56, (60, 40.0, 0.5, 0.75, 56), [], {"camera"}, None),
                ("style 56 without the carry (--tol cam_type_carry=0 alone: isolates the carry)", ACC56, (60, 40.0, 0.5, 0.75, 56), ["--tol", "cam_type_carry=0"], {"camera", "cam-tslack"}, None),
                ("the same plus a real 9-unit centre error on F + 4 (down, with the lag)", ACC, (60, 40.0, 0.5, 0.75, 2, -9.0, 4), [], {"camera", "cam-tslack"}, None),
                ("the same plus a real 4-unit centre error on F + 10 (after the carry)", ACC, (60, 40.0, 0.5, 0.75, 2, 4.0, 10), [], {"camera", "cam-tslack"}, None),
                ("the same lag in a style without centre ratios (event camera 4: no carry)", ACC4, (60, 40.0, 0.5, 0.75, 4), [], {"camera", "cam-tslack"}, None)):
            B = os.path.join(tmp, "bccarry" + str(abs(hash(name))))
            write_run(B, 2, ccarry=kw)
            rc, gates, out = cmp(a, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (note is not None and note not in out):
                fails.append("%s: failed gates %s (want %s), note %r\n%s" % (name, sorted(failed), sorted(want), note, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the carrier-offset rule (6j) (pair): Link (and the camera) ride the ship of the 6i sail-up
        # shift (B 20 units ahead, then the turn drift): on board (procedure 136) link-tslack and cam-tslack pass with
        # the carrier's capped offset subtracted; the same plus a real 20-unit Link / camera error on the ship fails;
        # not on a carrier (procedure 5, same positions) no term: fails
        AR = os.path.join(tmp, "a30ride")
        write_run(AR, 1, ship=(40, 60, 300, None, None, (30, False)), ride=(136, None))
        AR5 = os.path.join(tmp, "a30ride5")
        write_run(AR5, 1, ship=(40, 60, 300, None, None, (30, False)), ride=(5, None))
        for name, a, kw, bad in (("Link and camera on the ship (rule note 6j, pp5-like)", AR, dict(ride=(136, None)), set()),
                                 ("on the ship plus a real 20-unit Link / camera error", AR, dict(ride=(136, (70, 20.0))), {"link-tslack", "cam-tslack"}),
                                 ("the same positions off a carrier (no term)", AR5, dict(ride=(5, None)), {"link-tslack", "cam-tslack"}),
                                 # an unconverted Link (trace dt 1) riding the converted ship (pr1 / pj2 /: CONV 0xA5,0x26):
                                 # the carrier's offset is the carrier's, not Link's: the same term
                                 ("unconverted Link on the converted ship (rule note 6j, the rider's dt does not matter)", AR, dict(ride=(136, None), link_dt1=True), set()),
                                 ("unconverted Link on the ship plus a real 20-unit error", AR, dict(ride=(136, (70, 20.0)), link_dt1=True), {"link-tslack", "cam-tslack"})):
            B = os.path.join(tmp, "bride%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=(40, 60, 300, None, None, (30, True)), **kw)
            rc, gates, out = cmp(a, B)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad or gates.get("ship") != "PASS":
                fails.append("%s: failed slack gates %s (want %s), ship %s\n%s" % (name, sorted(got), sorted(bad), gates.get("ship"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the camera on the ship (pairs 960 / 630): a ride-style camera (style 36,
        # its eye 1500 behind the centre along the ship's heading) on the turning ship of the route: B's heading
        # drift (accepted by the ship gate) turns the eye around the centre by R x |facing B - A|; cam-tslack passes with
        # the ship term, fails without it (--tol ship_cam=0); a real 3-degree yaw error of B's eye beyond the ship's
        # heading drift fails; the same eye in a non-ride style (90, follow) gets no yaw term and fails
        ARC = os.path.join(tmp, "a30ridecam")
        write_run(ARC, 1, ship=(40, 60, 300, None, None, (30, False)), ride=(136, None, None, (1500.0, 36, 0.0, 0)))
        ARC9 = os.path.join(tmp, "a30ridecam90")
        write_run(ARC9, 1, ship=(40, 60, 300, None, None, (30, False)), ride=(136, None, None, (1500.0, 90, 0.0, 0)))
        for name, a, rc_, extra, bad in (
                ("ride camera on the ship with a 240-unit heading drift after the turn (eye arc 34.5, over the eye drift; rule 2026-11-18)", ARC, (1500.0, 36, 0.0, 0, (240, 65)), [], set()),
                ("the same without the ship term (--tol ship_cam=0)", ARC, (1500.0, 36, 0.0, 0, (240, 65)), ["--tol", "ship_cam=0"], {"cam-tslack"}),
                ("the same plus a real 3-degree eye yaw error from frame 70", ARC, (1500.0, 36, 3.0, 70, (240, 65)), [], {"cam-tslack"}),
                ("the same eye in a follow style (no yaw term)", ARC9, (1500.0, 90, 0.0, 0, (240, 65)), [], {"cam-tslack"})):
            B = os.path.join(tmp, "bridecam%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=(40, 60, 300, None, None, (30, True)), ride=(136, None, None, rc_))
            rc, gates, out = cmp(a, B, *extra)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad or gates.get("ship") != "PASS":
                fails.append("%s: failed slack gates %s (want %s), ship %s\n%s" % (name, sorted(got), sorted(bad), gates.get("ship"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the carrier-offset rule (6j) addendum (evship_cam / evcam_offship default on): Link rides the ship of the
        # fst8-like DEMO_MOVE arrival (6h's drift puts B up to 9 ahead, B stops first; the ship gate passes) while an
        # event camera (type 3, style 4) has the ship as its centre target and an eye that does not take B's offset
        # (the pair's pull-back eye): the centre passes with 6j's subtraction, the eye is off by the ship's offset after it
        # (8 at frame 54 against 4.1): passes with the term, fails without it (--tol evship_cam=0); a real 15-unit eye
        # error from frame 40 fails; the same camera as a follow camera (type 12, style 90) gets no term. evcam_offship
        # (ruled in): an event camera with a fixed centre off the ship (587-973) on the route (B's offset
        # kept for longer than the ship-camera form's 30-frame window, as the pair's 7.8 for 380 frames) does not ride the
        # carrier: passes with it, fails without it (6j subtracts the ship's offset from a camera that has none); a real
        # 15-unit eye error fails with it
        evr = (FRAMES, FRAMES, 0, None, None, None, None, None, None, None, None, None, None, (0.0, 0, 0.0, 7))
        AEV = os.path.join(tmp, "a30evcam")
        write_run(AEV, 1, ship=evr, ride=(136, None))
        eyes = write_evcam(AEV, 1, True)
        AEV12 = os.path.join(tmp, "a30evcam12")
        write_run(AEV12, 1, ship=evr, ride=(136, None))
        write_evcam(AEV12, 1, True, typ=12, sty=90)
        evp = ((40, 60, 300, None, None, (30, False)), (40, 60, 300, None, None, (30, True)))
        AEVO = os.path.join(tmp, "a30evcamoff")
        write_run(AEVO, 1, ship=evp[0], ride=(136, None))
        write_evcam(AEVO, 1, False)
        EVON, EVOFF = [], []  # both default on
        for name, a, ev, extra, bad, note in (
                ("event camera on the ship, fixed eye, with the ship offset term (default)", AEV, (True, None, 3, 4), EVON, set(), "event camera on the ship (6j addendum, evship_cam): ship offset term on"),
                ("the same without the term (--tol evship_cam=0)", AEV, (True, None, 3, 4), ["--tol", "evship_cam=0"], {"cam-tslack"}, None),
                ("the same with the term plus a real 15-unit eye error from frame 40", AEV, (True, (40, 15.0), 3, 4), EVON, {"cam-tslack"}, None),
                ("the same camera as a follow camera (type 12, style 90): no term", AEV12, (True, None, 12, 90), EVON, {"cam-tslack"}, None),
                ("event camera off the ship (fixed centre and eye) with evcam_offship (default)", AEVO, (False, None, 3, 4), EVOFF, set(), "off the ship, carrier offset not subtracted (evcam_offship) on"),
                ("the same without evcam_offship (--tol evcam_offship=0: 6j subtracts the ship offset)", AEVO, (False, None, 3, 4), ["--tol", "evcam_offship=0"], {"cam-tslack"}, None),
                ("the same with evcam_offship plus a real 15-unit eye error from frame 40", AEVO, (False, (40, 15.0), 3, 4), EVOFF, {"cam-tslack"}, None)):
            B = os.path.join(tmp, "bevcam%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=evr if a != AEVO else evp[1], ride=(136, None))
            write_evcam(B, 2, ev[0], ev[1], ev[2], ev[3], eyes)
            rc, gates, out = cmp(a, B, *extra)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad or gates.get("ship") != "PASS" or (note is not None and note not in out):
                fails.append("%s: failed slack gates %s (want %s), ship %s, note %r\n%s" % (name, sorted(got), sorted(bad), gates.get("ship"), note, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # (pair frame 615): an event camera (type 3 / style 4) identical in both runs while
        # Link rides the ship of the route (B's ship 20 ahead): 6j's first form subtracts the carrier offset from
        # B's camera and fails; the event camera's centre is off the ship (2026-11-19: chosen by target, evship_r, never
        # the better of both forms), so it takes no carrier offset and passes; without the
        # rule (--tol ev_cam_off=0) it fails; the same plus a real 5-unit eye error (y, off the offset's xz) fails; the
        # same identical camera in a ride style (36, not an event camera) gets no such form and fails (the error starts
        # after the camera stands still, so the held-direction latch term of sv2 does not take it)
        def evcam(d, n, err=0.0, sty=4):
            def rec(c, fr):
                if fr >= 5:
                    # a fixed shot high above the route (the centre never near the ship; evship_r 300)
                    c[5:11] = ["500.000", "%.3f" % (3200.0 + (err if n > 1 and fr >= 80 else 0.0)), "1200.000", "500.000", "3000.000", "1500.000"]
            # the pair's camera: type 3 (event) with style 4; a ride-style camera keeps the follow type
            post_cam(d, n, rec=rec, style=lambda fr: sty if fr >= 5 else 0, ctype=lambda fr: 3 if fr >= 5 and sty == 4 else 12)
        AEV = os.path.join(tmp, "a30evoff")
        write_run(AEV, 1, ship=(40, 60, 300, None, None, (30, False)), ride=(136, None))
        evcam(AEV, 1)
        AEV36 = os.path.join(tmp, "a30evoff36")
        write_run(AEV36, 1, ship=(40, 60, 300, None, None, (30, False)), ride=(136, None))
        evcam(AEV36, 1, sty=36)
        for name, a, err, sty, extra, bad in (
                ("event camera identical in both runs while Link rides the ship, centre off the ship (fst8 615: no carrier offset)", AEV, 0.0, 4, [], set()),
                ("the same without the rule (--tol ev_cam_off=0)", AEV, 0.0, 4, ["--tol", "ev_cam_off=0"], {"cam-tslack"}),
                ("the same event camera plus a real 5-unit eye error from frame 80", AEV, 5.0, 4, [], {"cam-tslack"}),
                ("the same identical camera in a ride style (36: not an event camera)", AEV36, 0.0, 36, [], {"cam-tslack"})):
            B = os.path.join(tmp, "bevoff%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=(40, 60, 300, None, None, (30, True)), ride=(136, None))
            evcam(B, 2, err, sty)
            rc, gates, out = cmp(a, B, *extra)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad or gates.get("ship") != "PASS" or (not bad and not extra and "event camera off the ship without the carrier offset (fst8 rule, by target) on 0 " in out):
                fails.append("%s: failed slack gates %s (want %s), ship %s\n%s" % (name, sorted(got), sorted(bad), gates.get("ship"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the held camera-gap rule (6m), held-gap angle allowance (talk16 / talk18 847): an event camera (style 4)
        # about a still centre, B's eye direction 0.12 deg off A's from frame 40 at R 300 (0.63 units), then camera_Stop's
        # zoom-out R 300 -> 1600 over frames 60-70 (3.35 units at 1600): judged by angle it passes; without the rule
        # (--tol held_gap=0) it fails; a gap that opens only after the zoom (frame 75), one that grows after it (0.12 ->
        # 0.3 deg from frame 80) and the held gap with B's radius 10 longer fail. B's Link at dt 1 (the event fallback, as
        # in talk16: the camera's 6c eye drift term, which grows with a radius change at dt < 1, stays out)
        def hgcam(d, n, th=0.12, at=40, th2=None, at2=None, dr=0.0):
            def rec(c, fr):
                R = 300.0 if fr < 60 else 1600.0 if fr >= 70 else 300.0 + 130.0 * (fr - 60)
                psi = 0.0
                if n > 1:
                    R += dr if fr >= 70 else 0.0
                    psi = math.radians(th2 if at2 is not None and fr >= at2 else th if fr >= at else 0.0)
                c[5:11] = ["%.4f" % (R * math.sin(psi)), "150.000", "%.4f" % (-R * math.cos(psi)), "0.000", "150.000", "0.000"]
            post_cam(d, n, rec=rec, style=lambda fr: 4)
        AHG = os.path.join(tmp, "a30heldgap")
        write_run(AHG, 1)
        hgcam(AHG, 1)
        for name, kw, extra, bad in (
                ("event camera gap 0.12 deg held from R 300 through a zoom-out to R 1600 (rule note 6m)", {}, [], set()),
                ("the same without the rule (--tol held_gap=0)", {}, ["--tol", "held_gap=0"], {"cam-tslack"}),
                ("a gap that opens only after the zoom (0.12 deg at R 1600 from frame 75)", dict(at=75), [], {"cam-tslack"}),
                ("a held gap that grows after the zoom (0.12 -> 0.3 deg from frame 80)", dict(th2=0.3, at2=80), [], {"cam-tslack"}),
                ("the held gap with B's radius 10 longer after the zoom", dict(dr=10.0), [], {"cam-tslack"})):
            B = os.path.join(tmp, "bheldgap%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, link_dt1=True)
            hgcam(B, 2, **kw)
            rc, gates, out = cmp(AHG, B, *extra)
            got = {g for g in ("cam-tslack",) if gates.get(g) == "FAIL"}
            if got != bad or (not bad and "held-gap angle allowance (rule note 6m) on 0 " in out):
                fails.append("%s: failed slack gates %s (want %s)\n%s" % (name, sorted(got), sorted(bad), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # event-start world sound (frame 61; the 8a / 8c family of
        # sw61a's freeze-frame world sounds): the process manager's per-frame world sound 107D (fpcM_Management 025DFA24,
        # with its wrapper-3 pointer record) plays every frame up to Link's FoodThrow event start; both cameras enter the
        # event style 4 on frame 61; B's event starts inside 61, so its series ends at 60, A's at 61: the two A-only starts
        # pass. Without the rule (--tol ev_start_se=0), from a process's code (024424FC), off the event-start frame
        # (event camera from 63), a lone start (no series in either run) and two A-only starts of the key on 61 fail
        def evs_run(d, n, last, caller="025DFA24", ev=61, first=50, dup=False):
            write_run(d, n)
            post_cam(d, n, style=lambda fr: 4 if fr >= ev else 0)
            rows = []
            for f in range(first, last + 1):
                st = 1000 - 5 * n + (f + 5) * n + n - 1
                rows += ["%d 1 3 47C256B8 %s" % (st, caller), "%d 1 0 0000107D %s" % (st, caller)]
                if dup and f == last:
                    rows.append("%d 1 0 0000107D %s" % (st, caller))
            open(os.path.join(d, "se.txt"), "a").write("\n".join(rows) + "\n")
        for name, ka, kb, extra, bad in (
                ("event-start world sound 107D A-only on the event-start frame (rule 2026-11-18, esa egr1 61)", dict(last=61), dict(last=60), [], set()),
                ("the same without the rule (--tol ev_start_se=0)", dict(last=61), dict(last=60), ["--tol", "ev_start_se=0"], {"sound"}),
                ("the same series from a process's code (024424FC, not a world sound)", dict(last=61, caller="024424FC"), dict(last=60, caller="024424FC"), [], {"sound"}),
                ("the same A-only start off the event-start frame (event camera from 63)", dict(last=61, ev=63), dict(last=60, ev=63), [], {"sound"}),
                ("a lone world sound A-only on the event-start frame (no per-frame series)", dict(last=61, first=61), dict(last=0, first=1), [], {"sound"}),
                ("two A-only starts of 107D on the event-start frame", dict(last=61, dup=True), dict(last=60), [], {"sound"})):
            A = os.path.join(tmp, "aevs%d" % len(bnames))
            B = os.path.join(tmp, "bevs%d" % len(bnames))
            bnames[name] = B
            evs_run(A, 1, **ka)
            evs_run(B, 2, **kb)
            rc, gates, out = cmp(A, B, *extra)
            got = {g for g in ("sound",) if gates.get(g) == "FAIL"}
            if got != bad or (not bad and "event-start frame in one run only" not in out):
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(got), sorted(bad), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the speed-ramp position-gap rule (6c) contact propagation (real cases kmoD1_2 46-52: link-tslack
        # over 0-3 s 1.84 / tol 2 with the term, 4.26 without; tnkD1_2 166 launch; 39-46 push 9.7 > allowance 6.8):
        # an enemy 300 away pushes Link 7 units over frames 46-52; B pushed 3 further, its enemy 1.8 further and facing 200
        # s16 off (allowance 1.8 + 200 pi / 32768 x 300 = 7.55): link-tslack passes with the carried offset; without the
        # term (--tol contact=0) it fails; a push 10 further (over the allowance) fails; the enemy failing its own facing
        # word (300 s16 > its 256 tolerance) gives no term; no --admp-spec, no term; a push across the enemy direction (z,
        # not radial) is no contact
        ACT = os.path.join(tmp, "a30contact")
        write_run(ACT, 1)
        post_contact(ACT, 1)
        ACTZ = os.path.join(tmp, "a30contactz")
        write_run(ACTZ, 1)
        post_contact(ACTZ, 1, axis=2)
        ON = ["--tol", "contact=1"]
        for name, a, kw, extra, bad in (
                ("enemy push with the enemy's 6c gap: Link 3 further, allowance 7.55 (rule note 6c contact propagation, --tol contact=1)", ACT, dict(extra=3.0), ON, set()),
                # opt-in (2026-11-18, after: a knock-back gap counted as enemy drift was a real defect)
                ("the same by default (the term is opt-in: off)", ACT, dict(extra=3.0), [], {"link-tslack"}),
                ("the same without the term (--tol contact=0)", ACT, dict(extra=3.0), ["--tol", "contact=0"], {"link-tslack"}),
                ("a push 10 further (over the enemy's allowance)", ACT, dict(extra=10.0), ON, {"link-tslack"}),
                ("the enemy fails its own facing word (300 s16 > 256): no term", ACT, dict(extra=3.0, face=300), ON, {"link-tslack"}),
                ("no --admp-spec: no term", ACT, dict(extra=3.0), None, {"link-tslack"}),
                ("a push across the enemy direction (z, not radial): no contact", ACTZ, dict(extra=3.0, axis=2), ON, {"link-tslack"})):
            B = os.path.join(tmp, "bcontact%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2)
            post_contact(B, 2, **kw)
            args = (["--admp-spec", os.path.join(B, "en.admp.spec")] + extra) if extra is not None else ON
            rc, gates, out = cmp(a, B, *args)
            got = {g for g in ("link-tslack",) if gates.get(g) == "FAIL"}
            if got != bad or (not bad and "contact: frames 46-52 (push" not in out):
                fails.append("%s: failed slack gates %s (want %s)\n%s" % (name, sorted(got), sorted(bad), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the carrier-offset rule (6j) with the 6i addendum (pairs): Link rides the ship through the sail's branch
        # change (B up to ~36 units ahead, inside the addendum's xz term): the carrier cap takes that term too, so
        # link-tslack / cam-tslack pass; the same plus a real 20-unit Link / camera error fails
        ABRR = os.path.join(tmp, "a30brride")
        brs = (FRAMES, FRAMES, 0, None, None, None, None, None, None, None, (30, 50, None))
        write_run(ABRR, 1, ship=brs, ride=(136, None))
        for name, kw, bad in (("Link on the ship through the sail's branch change (6j cap with the 6i addendum term)", dict(ride=(136, None)), set()),
                              ("the same plus a real 20-unit Link / camera error", dict(ride=(136, (70, 20.0))), {"link-tslack", "cam-tslack"})):
            B = os.path.join(tmp, "bbrride%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=brs, **kw)
            rc, gates, out = cmp(ABRR, B)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad or gates.get("ship") != "PASS":
                fails.append("%s: failed slack gates %s (want %s), ship %s\n%s" % (name, sorted(got), sorted(bad), gates.get("ship"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # (the carrier-offset rule (6j) y): Link inherits the ship's y offset capped by rule (c)'s
        # plausibility band, not by the xz bound: B's sea 2 frames ahead (ship y up to 12.6 off, inside A's band) on a
        # straight cruise (xz bound 2) passes; a real 20-unit Link y error on top fails
        AY = os.path.join(tmp, "a30sea")
        write_run(AY, 1, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, (60.0, 0.0)), ride=(136, None))
        for name, kw, bad in (("Link on the ship, B's sea 2 frames ahead (Q9: y offset inside rule (c)'s band)", dict(ride=(136, None)), set()),
                              ("the same plus a real 20-unit Link y error", dict(ride=(136, None, (50, 20.0))), {"link-tslack"})):
            B = os.path.join(tmp, "bsea%d" % len(bad))
            write_run(B, 2, ship=(FRAMES, FRAMES, 0, None, None, None, None, None, None, (60.0, 2.0)), **kw)
            rc, gates, out = cmp(AY, B)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad or gates.get("ship") != "PASS":
                fails.append("%s: failed slack gates %s (want %s), ship %s\n%s" % (name, sorted(got), sorted(bad), gates.get("ship"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # 6h's speed part on a long cruise (ramp 0 -> 40 over 10-19, then 70 frames at a steady speed): adds nothing
        # after the ramp; and the start + stop's two episodes add |40| + |-39.65|
        AC = os.path.join(tmp, "a30shipc")
        write_run(AC, 1, ship=(FRAMES, FRAMES, 0, None))
        BC = os.path.join(tmp, "bshipc")
        write_run(BC, 2, ship=(FRAMES, FRAMES, 0, None))
        dcr = c60.ship_turn_tol(c60.ship_rows(os.path.join(AC, "ship.admp"), c60.Frames(AC, 1000), None),
                                c60.ship_rows(os.path.join(BC, "ship.admp"), c60.Frames(BC, 1000), None), 0.25)
        dss = c60.ship_turn_tol(c60.ship_rows(os.path.join(AS, "ship.admp"), c60.Frames(AS, 1000), None),
                                c60.ship_rows(os.path.join(bnames["ship start + stop, the sail-down on B's first step (rule notes 6h episodes, 6i(i))"], "ship.admp"), c60.Frames(AS, 1000), None), 0.25)
        if not (abs(dcr.get(20, 0) - 10.0) < 0.01 and dcr.get(89) == dcr.get(20) and abs(dss.get(30, 0) - 10.0) < 0.01 and dss.get(89, 0) > 19.8):
            fails.append("6h speed episodes: cruise %r at 20 / %r at 89, start + stop %r at 30 / %r at 89" % (dcr.get(20), dcr.get(89), dss.get(30), dss.get(89)))
        else:
            print("ok   6h speed episodes: a cruise adds nothing after its ramp (%.3g at 20 and 89); start + stop %.3g -> %.3g" % (dcr[20], dss[30], dss[89]))
        # the drift term itself: held on the straight run after a turn; after the opposite turn back to the speed term
        # only (0.25 x 40 = 10: the net speed change), i.e. bounded
        def drift(a, name):
            fa_, fb_ = c60.Frames(a, 1000), c60.Frames(bnames[name], 1000)
            return c60.ship_turn_tol(c60.ship_rows(os.path.join(a, "ship.admp"), fa_, None),
                                     c60.ship_rows(os.path.join(bnames[name], "ship.admp"), fb_, None), 0.25)
        dh = drift(AH2, "ship straight run after a turn (the turn's offset held)")
        dc = drift(AH3, "ship turn and opposite turn (cancel)")
        if not (dh.get(61, 0) > 15 and abs(dh.get(85, 0) - dh.get(61, 0)) < 1e-9 and abs(dc.get(85, 0) - 10.0) < 0.01 and dc.get(55, 0) > 13):
            fails.append("6h drift term: hold %r at 61 / %r at 85, cancel %r at 55 / %r at 85" % (dh.get(61), dh.get(85), dc.get(55), dc.get(85)))
        else:
            print("ok   6h drift term: held %.3g after the turn; turn + opposite turn %.3g -> %.3g (the net speed term)" % (dh[85], dc[55], dc[85]))
        # the speed-ramp position-gap rule (6c) camera, centre chase (pkk1d_4 / pkk3d_4 frame 54): pkk1d_4's measured
        # centres (N 4) while the centre chases the lock-on target pass cam-tslack with the g |v| term only (--tol cchase=0
        # fails); a real error of B's centre beyond the term at frame 54 (5 units in x, past tol 2 + 0.27 x 10.5 = 4.83)
        # fails; the same centres with no lock-on (the centre follows Link) get no term and fail
        ACC = os.path.join(tmp, "a30cchase")
        write_run(ACC, 1, cchase=("lock", 0.0))
        ACF = os.path.join(tmp, "a30cfollow")
        write_run(ACF, 1, cchase=("follow", 0.0))
        for name, a, kw, extra, want, note in (
                ("6c centre chase: pkk1d_4's centres, locked on (N 4)", ACC, ("lock", 0.0), [], {"camera"}, "g |v| on "),
                ("6c centre chase: the same with the term off (--tol cchase=0)", ACC, ("lock", 0.0), ["--tol", "cchase=0"], {"camera", "cam-tslack"}, None),
                ("6c centre chase: plus a real 5-unit centre error at frame 54", ACC, ("lock", 5.0), [], {"camera", "cam-tslack"}, None),
                ("6c centre chase: the same centres following Link (no lock-on: no term)", ACF, ("follow", 0.0), [], {"camera", "cam-tslack"}, "g |v| on 0 frame(s)")):
            B = os.path.join(tmp, "bcchase" + str(abs(hash(name))))
            write_run(B, 4, cchase=kw)
            rc, gates, out = cmp(a, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (note is not None and note not in out):
                fails.append("%s: failed gates %s (want %s), note %r\n%s" % (name, sorted(failed), sorted(want), note, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the per-step camera-collision rule (6b) episode (crawl pair cy): cy7's measured eye / centre pass cam-tslack with the
        # episode window only (frames 202-227: B's wall release at 207 a jump A makes 20 frames later; converged at 228;
        # --tol bump_ep=0 fails); a gap that does not converge (B's eye 20 off in x from cy7 228 on) gets no window and fails
        ACB = os.path.join(tmp, "a30cbump")
        write_run(ACB, 1, cbump=(0.0,))
        for name, a, kw, extra, want, note in (
                ("6b episode: cy7's crawl wall release (N 2)", ACB, (0.0,), [], {"camera"}, "converged at "),
                ("6b episode: the same with the window off (--tol bump_ep=0)", ACB, (0.0,), ["--tol", "bump_ep=0"], {"camera", "cam-tslack"}, None),
                ("6b episode: a gap that does not converge (B's eye 20 off from cy7 228 on)", ACB, (20.0, 62), [], {"camera", "cam-tslack"}, "not converged within 30")):
            B = os.path.join(tmp, "bcbump" + str(abs(hash(name))))
            write_run(B, 2, cbump=kw)
            rc, gates, out = cmp(a, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (note is not None and note not in out):
                fails.append("%s: failed gates %s (want %s), note %r\n%s" % (name, sorted(failed), sorted(want), note, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the speed-ramp position-gap rule (6c) camera, Link-following centre chase (himo2 / r58a hungCamera 175-183):
        # himo2's measured centres (N 2) in hungCamera style 35 pass cam-tslack with the per-axis g |v| term only (--tol
        # cchase=0 fails: 2.04 > 2 at 180 as in r58a); a real 5-unit centre error at frame 180 fails; the same centres in a
        # style without a centre follow ratio (0) get no term and fail
        AHU = os.path.join(tmp, "a30chung")
        write_run(AHU, 1, chung=(0.0,))
        AHU0 = os.path.join(tmp, "a30chung0")
        write_run(AHU0, 1, chung=(0.0, 46, 0))
        for name, a, kw, extra, want, note in (
                ("6c Link-following centre chase: himo2's hungCamera centres (N 2)", AHU, (0.0,), [], {"camera"}, "g |v| per axis on "),
                ("6c Link-following centre chase: the same with the term off (--tol cchase=0)", AHU, (0.0,), ["--tol", "cchase=0"], {"camera", "cam-tslack"}, None),
                ("6c Link-following centre chase: plus a real 5-unit centre error at himo2 180", AHU, (5.0,), [], {"camera", "cam-tslack"}, None),
                ("6c Link-following centre chase: the same centres in a style without follow ratios (no term)", AHU0, (0.0, 46, 0), [], {"camera", "cam-tslack"}, "g |v| per axis on 0 frame(s)")):
            B = os.path.join(tmp, "bchung" + str(abs(hash(name))))
            write_run(B, 2, chung=kw)
            rc, gates, out = cmp(a, B, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or (note is not None and note not in out):
                fails.append("%s: failed gates %s (want %s), note %r\n%s" % (name, sorted(failed), sorted(want), note, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the speed-ramp position-gap rule (6c) camera (yaw episodes): the pair's measured manualCamera yaw (B 3.7
        # deg past A after the first release, 3.79 after the second turn; the term 3.7 / 5.69 deg) passes cam-tslack only
        # with the term (--tol myaw=0 fails); a real 5 deg error of B's view (the drift's direction) after both turns
        # (160) or on the hold after the first release (106) fails; the same view with no stick (still) adds
        # nothing (a 3 deg error then fails); the pair's yaw in a follow style (no manualCamera) gets no term and fails
        AY = os.path.join(tmp, "a30myaw")
        write_run(AY, 1, myaw=("pman2", 0.0))
        AYS = os.path.join(tmp, "a30myaws")
        write_run(AYS, 1, myaw=("still", 0.0))
        AYF = os.path.join(tmp, "a30myawf")
        write_run(AYF, 1, myaw=("follow", 0.0))
        for yi, (name, a, kw, extra, want) in enumerate((
                ("6c camera yaw: pman2 (manualCamera stick ramps)", AY, ("pman2", 0.0), [], {"camera"}),
                ("6c camera yaw: pman2 with the term off", AY, ("pman2", 0.0), ["--tol", "myaw=0"], {"camera", "cam-tslack"}),
                ("6c camera yaw: pman2 plus a real 5 deg error after both turns", AY, ("pman2", -5.0), [], {"camera", "cam-tslack"}),
                ("6c camera yaw: pman2 plus a real 5 deg error on the hold after the first release", AY, ("pman2", -5.0, 50), [], {"camera", "cam-tslack"}),
                ("6c camera yaw: manual style, no stick input (no term)", AYS, ("still", 0.0), [], set()),
                ("6c camera yaw: manual style, no stick input, a 3 deg error", AYS, ("still", -3.0), [], {"camera", "cam-tslack"}),
                ("6c camera yaw: pman2's yaw in a follow style (no term)", AYF, ("follow", 0.0), [], {"camera", "cam-tslack"}))):
            B = os.path.join(tmp, "bmyaw%d" % yi)
            write_run(B, 2, myaw=kw)
            rc, gates, out = cmp(a, B, *extra, end="4.1")
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want:
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # the animation-phase lead rule (6d(c)): pgrab5's change frame (120 -> 70 here): B's start pose leads
        # A's by 0.858, the change resets both (end gap 0.062): term 0.919 on that frame only, tol 2.893 + 0.919 >= 3.133
        # passes; a real 1.0 speed error of B's frame on top fails; without the change (both animations played on) no
        # term on any frame; a base foot that differs gets no term (reported) and fails
        sys.path.insert(0, HERE)
        import compare60 as c6
        # (295-297): a carry window that starts on B's early event frame counts B's switch there
        # (the event's own procedure init); A's switch a frame later pairs with it; a missing B switch still fails
        for name, swb, want_pl, want_note in (
                ("6l (b) window: B's switch on the window's first frame pairs with A's a frame later", {295: 1}, {295, 296}, "1 a frame apart"),
                ("6l (b) window: no switch in B fails (reported)", {}, set(), "differ"),
                ("6l (b) window: B's switch two frames before A's fails", {294: 1}, set(), "differ")):
            pl, notes = c6.carried_anim_switch({296: 1}, swb, [(295 if swb != {294: 1} else 294, 297, None)])
            if pl != want_pl or not any(want_note in n for n in notes):
                fails.append("%s: plausibility %s, notes %s" % (name, sorted(pl), notes))
            else:
                print("ok   %s" % name)
        # (346): a sound object's start (wrapper 4, r4 a heap pointer) keyed by a stale guest caller
        # pairs by the sound id of its wrapper-0 line; another id does not pair
        SEA = os.path.join(tmp, "se_w4a")
        SEB = os.path.join(tmp, "se_w4b")
        for d_, rows in ((SEA, ["100 1 4 47C3EF3C 0242F5B4", "100 1 0 00003809 0242F5B4"]),
                         (SEB, ["200 0 4 47C3EF3C 0240D5D0", "200 0 0 00003809 0240D5D0", "201 1 4 47C3EF3C 0240D5D0",
                                "201 1 0 00004000 0240D5D0"])):
            os.makedirs(d_)
            open(os.path.join(d_, "se.txt"), "w").write("\n".join(rows) + "\n")
        class _F:
            def __init__(self, o): self.o = o
            def frame(self, st): return st - self.o
        ia, ib = c6.se_w4_ids(SEA, _F(0), None), c6.se_w4_ids(SEB, _F(100), None)
        ka, kb, kb2 = ("4", "ptr", "0242F5B4"), ("4", "ptr", "0240D5D0"), ("4", "ptr", "0240D5D0")
        ua, ub, n = c6.se_w4_pair([(100, ka)], [(100, kb)], ia, ib, 1)
        if (ua, ub, n) != ([], [], 1):
            fails.append("se_w4 pairing, same id: %s %s %d (ids %s / %s)" % (ua, ub, n, ia, ib))
        else:
            print("ok   a sound object's start with a stale caller pairs by its sound id (psword2g 346)")
        ua, ub, n = c6.se_w4_pair([(100, ka)], [(101, kb2)], ia, [x for x in ib if x[0] == 101], 1)
        if n != 0 or len(ua) != 1 or len(ub) != 1:
            fails.append("se_w4 pairing, another id paired: %s %s %d" % (ua, ub, n))
        else:
            print("ok   a sound object's start with another sound id stays unmatched")
        FC = os.path.join(tmp, "a30fchg")
        write_run(FC, 1, fchg=(65, "chg", 0.0))
        FN = os.path.join(tmp, "a30fnochg")
        write_run(FN, 1, fchg=(65, "nochg", 0.0))
        FR = os.path.join(tmp, "a30fr05")
        write_run(FR, 1, fchg=(65, "r05", 0.0))
        FO = os.path.join(tmp, "a30fnor")
        write_run(FO, 1, fchg=(65, "nor", 0.0))
        for name, a, kw, want, term in (("pgrab5's animation-change frame (rule note 6d(c))", FC, dict(fchg=(65, "chg", 0.0)), {"link-motion"}, {70: 0.919}),
                                        ("the same plus a real 1.0 speed error", FC, dict(fchg=(65, "chg", -1.0)), {"link-motion", "link-tslack"}, {70: 0.919}),
                                        ("a 0.3 speed error at the measured r 1: passes", FC, dict(fchg=(65, "chg", -0.3)), {"link-motion"}, {70: 0.919}),
                                        ("r = m3598 0.5 (column 32): term x 0.5", FR, dict(fchg=(65, "r05", 0.0)), {"link-motion"}, {70: 0.4597}),
                                        ("r = m3598 0.5 plus the 0.3 speed error: fails (r 1 passes it)", FR, dict(fchg=(65, "r05", -0.3)), {"link-motion", "link-tslack"}, {70: 0.4597}),
                                        ("no m3598 column (older trace): r taken as 1", FO, dict(fchg=(65, "nor", 0.0)), {"link-motion"}, {70: 0.919}),
                                        ("no animation change: no 6d(c) term", FN, dict(fchg=(65, "nochg", 0.0)), None, {}),
                                        ("base foot differs at the change: no term, reported", FC, dict(fchg=(65, "foot1", 0.0)), {"link-motion", "link-tslack"}, {})):
            B = os.path.join(tmp, "bfchg" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            fa, fb = c6.Frames(a, c6.origin(a)), c6.Frames(B, c6.origin(B))
            got, notes = c6.change_chord_tol(c6.link_rows(a, fa, None), B, fb, None)
            rc, gates, out = cmp(a, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            okterm = set(got) == set(term) and all(abs(got[f] - term[f]) < 0.002 for f in term)
            oknote = ("base foot" in out) == (kw["fchg"][1] == "foot1")
            if (want is not None and failed != want) or not okterm or not oknote:
                fails.append("%s: failed gates %s (want %s), term %s (want %s), notes %s\n%s" % (name, sorted(failed), sorted(want or ()), got, term, notes, out))
            else:
                print("ok   %s%s, term %s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else "", {f: round(v, 3) for f, v in got.items()}))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the cloth-simulation rule (6k): the cloth's statistics after the conversion, over >= 200 frames (mean position: >= 1800, the 6k window)
        AF = os.path.join(tmp, "a30cloth")
        write_run(AF, 1, frames=1900)
        write_cloth(AF, 1, "same", 1900)
        for ci, (name, mode, want, cend) in enumerate((
                ("cloth equal on every frame (rule note 6k)", "same", set(), "62.5"),
                ("cloth: per-vertex divergence with the same statistics (sf3's form, rule note 6k)", "chaos", set(), "62.5"),
                ("cloth: the flutter at double speed (p99 off)", "fast", {"cloth"}, "62.5"),
                ("cloth: one max sample 1.2 x the bound, p99 equal (6k amendment: passes)", "spike1.2", set(), "62.5"),
                ("cloth: one max sample 1.5 x the bound (6k amendment: max beyond 30 %)", "spike1.5", {"cloth"}, "62.5"),
                ("cloth: stuck after the conversion", "stuck", {"cloth"}, "62.5"),
                ("cloth: divergence over fewer than 200 frames", "chaos", {"cloth"}, "6"),
                ("cloth: divergence with the same statistics over 1000 frames (6k window: mean position needs 1800)", "chaos",
                 {"cloth"}, "35"))):
            BF = os.path.join(tmp, "bcloth%d" % ci)
            write_run(BF, 2, frames=1900)
            write_cloth(BF, 2, mode, 1900)
            rc, gates, out = cmp(AF, BF, end=cend)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or rc != (1 if want else 0) or gates.get("cloth") != ("FAIL" if want else "PASS"):
                fails.append("%s: failed gates %s (want %s), rc %d\n%s" % (name, sorted(failed), sorted(want), rc, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # 6k mean-position bound: |mean_B - mean_A| <= 3 sd_A sqrt(2 / N_eff), N_eff = window /
        # (2 tau_A), on a random flutter of gy13n4o's form (A one sample, B an independent one after the conversion)
        AN = os.path.join(tmp, "a30noise")
        write_run(AN, 1, frames=1900)
        write_cloth(AN, 1, "noise", 1900)
        for ci, (name, mode, want) in enumerate((
                ("cloth: an independent flutter sample of gy13n4o's form (6k bound: passes, N_eff reported)", "noise", set()),
                ("cloth: the same flutter, B's sail offset by 1 sd for the whole window (6k bound)", "noiseoff1", {"cloth"}),
                ("cloth: the flutter stuck after the conversion", "noisestuck", {"cloth"}))):
            BF = os.path.join(tmp, "bnoise%d" % ci)
            write_run(BF, 2, frames=1900)
            write_cloth(BF, 2, mode, 1900)
            rc, gates, out = cmp(AN, BF, end="62.5")
            failed = {g for g, st in gates.items() if st == "FAIL"}
            cl = [l for l in out.splitlines() if l.startswith("cloth")]
            if failed != want or rc != (1 if want else 0) or gates.get("cloth") != ("FAIL" if want else "PASS") or not any(
                    "N_eff" in l for l in cl) or not any("sail mean" in l for l in cl) or (
                    mode == "noiseoff1" and ("vertex 1: mean position" not in out or "sail mean (all 25 vertices)" not in out)):
                fails.append("%s: failed gates %s (want %s), rc %d\n%s" % (name, sorted(failed), sorted(want), rc, out))
            else:
                m = re.search(r"worst vertex \d+ [0-9.]+ apart = ([0-9.]+) x .*N_eff over the vertices ([0-9.]+ - [0-9.]+)\); "
                              r"sail mean [0-9.e+-]+ apart = ([0-9.]+) x .*N_eff ([0-9.]+)\)", " ".join(cl))
                print("ok   %s%s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else "",
                                      ", worst %s x the bound, N_eff %s; sail mean %s x, N_eff %s" % m.groups() if m else ""))
        # the animation-phase lead rule (6d(c)) addendum: pfol1's base-foot switch frame (107 -> 70 here):
        # A's chord foot 1, B's 0 (B switches on the frame's second step): B's foot's pose gaps 0.541 + A's own
        # foot-choice chord gap 2.631 = 3.174 (r 1), tol 0.636 + 3.174 >= 1.076 passes; a 1.0 speed error on top still
        # passes (the legitimate raw gap at this frame is 2.35: no sound bound fails it), 3.0 fails; B switching on the
        # first step too (chord feet equal, B's speed still of foot 0) gets the pose term only (0.161) and fails; only A
        # switching (B keeps foot 0): no term at 70, reported, fails (B's switch frame 71 gets its own pose term); no
        # switch at all: no term, fails
        FL = os.path.join(tmp, "a30fol")
        write_run(FL, 1, fchg=(65, "fol", 0.0))
        FLN = os.path.join(tmp, "a30folnosw")
        write_run(FLN, 1, fchg=(65, "folnosw", 0.0))
        for name, a, kw, want, term, note in (
                ("pfol1's base-foot switch frame (rule note 6d(c) addendum)", FL, dict(fchg=(65, "fol", 0.0)), {"link-motion"}, {70: 3.1737}, None),
                ("the same plus a 1.0 speed error: inside the bound, passes", FL, dict(fchg=(65, "fol", -1.0)), {"link-motion"}, {70: 3.1737}, None),
                ("the same plus a real 3.0 speed error", FL, dict(fchg=(65, "fol", -3.0)), {"link-motion", "link-tslack"}, {70: 3.1737}, None),
                ("B switches on the first step too (chord feet equal): pose term only", FL, dict(fchg=(65, "folsame", 0.0)), {"link-motion", "link-tslack"}, {70: 0.1611}, None),
                ("only A switches (chord feet differ): no term, reported", FL, dict(fchg=(65, "folaonly", 0.0)), {"link-motion", "link-tslack"}, {71: None}, "switches in A only"),
                ("no base-foot switch, no animation change: no term", FLN, dict(fchg=(65, "folnosw", 0.0)), {"link-motion", "link-tslack"}, {}, None)):
            B = os.path.join(tmp, "bfol" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            fa, fb = c6.Frames(a, c6.origin(a)), c6.Frames(B, c6.origin(B))
            got, notes = c6.change_chord_tol(c6.link_rows(a, fa, None), B, fb, None)
            rc, gates, out = cmp(a, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            okterm = set(got) == set(term) and all(term[f] is None or abs(got[f] - term[f]) < 0.002 for f in term)
            oknote = (note is None and not notes) or (note is not None and any(note in l for l in notes) and note in out)
            if failed != want or not okterm or not oknote:
                fails.append("%s: failed gates %s (want %s), term %s (want %s), notes %s\n%s" % (name, sorted(failed), sorted(want), got, term, notes, out))
            else:
                print("ok   %s%s, term %s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else "", {f: round(v, 3) for f, v in got.items()}))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # the strict RNG-pair rule (8d) tooling: the keyed RNG mode is printed; runs with different modes fail rng-mode
        SEQ, FRM = "[t60] test rng keyed mode: seq seed 2", "[t60] WWHD_TEST_RNG_KEYED=2: test random numbers on (mode 4), first draw frame 9"
        AQ = os.path.join(tmp, "a30seq")
        write_run(AQ, 1, rngmode=SEQ)
        for name, kw, want, text in (("both runs keyed seq", dict(rngmode=SEQ), set(), "A keyed seq 2, B keyed seq 2"),
                                     ("A keyed seq, B keyed frame (old log line)", dict(rngmode=FRM), {"rng-mode"}, "B keyed frame 2"),
                                     ("A keyed seq, B the game's RNG", dict(), {"rng-mode"}, "B game")):
            B = os.path.join(tmp, "bseq" + str(abs(hash(name))))
            write_run(B, 2, **kw)
            rc, gates, out = cmp(AQ, B)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or text not in out:
                fails.append("%s: failed gates %s (want %s, text %r)\n%s" % (name, sorted(failed), sorted(want), text, out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
        # the speed-ramp position-gap rule (6c) for an enemy's own motion: a knock-back flight (speed v0 set
        # on the hit, slowing by r per frame, A: decay then move the whole frame; B: per step, on the steps' own speeds)
        # leaves B (1 - dt) / 2 x sum |dv| ahead. The measured ChuChu flights (build38: cckU1_2 47C5CF50 14.445 with the
        # term 14.143, cckU2_2 47C5DE70 13.052 / 13.924 at N 2, 47C5DE70 19.552 / 21.130 at N 4) pass with the
        # 6c spec term; a gap 1.5 x the term fails; so does the measured gap when the speed is set on one step (no ramp
        # across the steps: term 0); the action word stays strict; the term also needs `side=enemy rule=6c` as a cause
        FSPEC = os.path.join(tmp, "fl.spec")
        open(FSPEC, "w").write("300 u8 0\n314 f32 2.0 6c=370\n31C f32 2.0 6c=370\n370 f32 0.5\n")

        def flight(d, n, sum_dv, gap=None, ramp=True, act_err=None):
            K, F0, v0 = 14, 20, 60.0
            r = sum_dv / K
            dt = 1.0 / n
            term = (1.0 - dt) / 2.0 * sum_dv if ramp else 0.0
            buf = bytearray()
            x = 0.0
            for f in range(FRAMES):
                for j in range(n):
                    k = j + 1
                    if F0 <= f < F0 + K:
                        if n == 1:
                            v = v0 - r * (f - F0 + 1)
                        else:
                            v = v0 - r * (f - F0) - (r * k / n if ramp else r)
                    else:
                        v = 0.0
                    x += v * dt
                    xx = x + ((gap - term) if gap is not None and f >= F0 + K - 1 and n > 1 else 0.0)
                    body = bytearray(0x380)
                    struct.pack_into(">B", body, 0x300, 1 + (f >= F0) + (1 if act_err is not None and f == act_err and n > 1 else 0))
                    struct.pack_into(">fff", body, 0x314, xx, 0.0, xx * 0.5)
                    struct.pack_into(">f", body, 0x370, v)
                    step = 1000 + f * n + j
                    buf += b"ADMP" + struct.pack("<QIfII", step, int(j == n - 1), dt, 0x30003000, len(body)) + body
            open(os.path.join(d, "fl.admp"), "wb").write(bytes(buf))
        AFS = {}
        for sdv in (56.57, 55.70, 56.35):  # A per flight (the measured sums of |dv|)
            AFS[sdv] = os.path.join(tmp, "a30fl%d" % int(sdv * 100))
            write_run(AFS[sdv], 1)
            flight(AFS[sdv], 1, sdv)
        for name, n, kw, extra, want in (("ChuChu flight cckU1_2 (N 2: 14.445, term 14.143)", 2, dict(sum_dv=56.57, gap=14.445), [], set()),
                                         ("ChuChu flight cckU2_2 (N 2: 13.052, term 13.924)", 2, dict(sum_dv=55.70, gap=13.052), [], set()),
                                         ("ChuChu flight cckU1_4 (N 4: 19.552, term 21.130)", 4, dict(sum_dv=56.35, gap=19.552), [], set()),
                                         ("flight gap 1.5 x the 6c term", 2, dict(sum_dv=56.57, gap=1.5 * 14.143), [], {"admp"}),
                                         ("flight gap 1.5 x the 6c term at N 4", 4, dict(sum_dv=56.35, gap=1.5 * 21.130), [], {"admp"}),
                                         ("measured gap with the speed set on one step (no ramp)", 2, dict(sum_dv=56.57, gap=14.445, ramp=False), [], {"admp"}),
                                         ("flight within the term, an action word differs", 2, dict(sum_dv=56.57, act_err=40), [], {"admp"}),
                                         ("side=enemy rule=6c cause", 2, dict(sum_dv=56.57), ["--strict-until", "60", "--cause", "side=enemy rule=6c decision=the flight's 6c gap moves the ChuChu's next jump on Link"], set())):
            B = os.path.join(tmp, "bfl" + str(abs(hash(name))))
            write_run(B, n)
            flight(B, n, **kw)
            rc, gates, out = cmp(AFS[kw["sum_dv"]], B, "--admp-spec", FSPEC, *extra)
            failed = {g for g, st in gates.items() if st == "FAIL"}
            if failed != want or ("6c term" not in out):
                fails.append("%s: failed gates %s (want %s)\n%s" % (name, sorted(failed), sorted(want), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(want)) if want else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # (camera): (1) the carrier chase term uses the style's
        # own centre follow ratio (style 36 BN07: 0.65 -> g 0.186 at N 2, against the fixed 0.75 -> 1/6) and, in a ride
        # style's entry transition, the share 1/(20 - t) (ptalk2 912-930); (2) an unconverted ride camera on a converted
        # ship within the ship gate takes the ship's attitude allowance (930 / talk13 965); (3) a held direction
        # latched within the time window is accepted for the hold (sv2 135)
        SHIPS = (FRAMES, FRAMES, 0, None, None)  # straight along +z, 40 a frame from frame 20
        def lat(lag, f0, f1=None):  # B's centre (and eye) lag across the motion (x), frames f0 .. f1 - 1
            def rec(c, f):
                if f0 <= f and (f1 is None or f < f1):
                    for k in (5, 8):
                        c[k] = "%.3f" % (float(c[k]) - lag)
            return rec
        AT36 = os.path.join(tmp, "a30ride36")
        write_run(AT36, 1, ship=SHIPS, ride=(136, None, None, (1200.0, 36, 0.0, 0)))
        AT90 = os.path.join(tmp, "a30ride9036")
        write_run(AT90, 1, ship=SHIPS, ride=(136, None, None, (1200.0, 36, 0.0, 0)))
        post_cam(AT90, 1, style=lambda f: 90 if f < 50 else 36)
        for name, a, lagr, sty, extra, bad in (
                ("carrier chase lag 9.0 in ride style 36 (g 0.186 from its ratio 0.65: tol 9.44; rule 2026-11-18, ptalk2)", AT36, lat(9.0, 50), None, [], set()),
                ("the same with the fixed ratio 0.75 (--tol ride_trans=0: tol 8.67)", AT36, lat(9.0, 50), None, ["--tol", "ride_trans=0"], {"cam-tslack"}),
                ("a real carrier lag of 12 in style 36", AT36, lat(12.0, 50), None, [], {"cam-tslack"}),
                ("lag 10.5 in style 36's entry transition t 0-14 (share 1/(20 - t): g >= 0.236)", AT90, lat(10.5, 50, 65), lambda f: 90 if f < 50 else 36, [], set()),
                ("the same lag without the transition share (--tol ride_trans=0)", AT90, lat(10.5, 50, 65), lambda f: 90 if f < 50 else 36, ["--tol", "ride_trans=0"], {"cam-tslack"}),
                ("lag 10.5 after the transition (t >= 20: the style's ratio only)", AT90, lat(10.5, 70), lambda f: 90 if f < 50 else 36, [], {"cam-tslack"})):
            B = os.path.join(tmp, "btrans%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=SHIPS, ride=(136, None, None, (1200.0, 36, 0.0, 0)))
            post_cam(B, 2, rec=lagr, style=sty)
            rc, gates, out = cmp(a, B, *extra)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad or gates.get("ship") != "PASS":
                fails.append("%s: failed slack gates %s (want %s), ship %s\n%s" % (name, sorted(got), sorted(bad), gates.get("ship"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # (2) 930 / talk13 965: the ship on a swell (B's sea a frame ahead: float y up to 3.1 off, inside A's band)
        # and pitching up to 35 s16 off A (period 34 frames, the pair's); B's unconverted ride camera (style 36, R 1200)
        # reads them in its latitude: 0.3 x d pitch + 0.8 x 0.5 (speed ratio) x atan(dy / 200), up to ~8 units
        SEA = (FRAMES, FRAMES, 0, None, None, None, None, None, None, (30.0, 0.0))
        SEAB = (FRAMES, FRAMES, 0, None, None, None, None, None, None, (30.0, 1.0))
        AATT = os.path.join(tmp, "a30att")
        write_run(AATT, 1, ship=SEA, ride=(136, None, None, (1200.0, 36, 0.0, 0)))
        ship_pitch(AATT, 1, 35.0, 34.0)
        post_cam(AATT, 1, unconv=True)
        def att(n_, err_deg=0.0):
            def wy(f, phi):
                return 30.0 * math.sin(2.0 * math.pi * (f + phi) / 60.0)
            def rec(c, f):
                if n_ == 1:
                    return
                dp = 35.0 * math.sin(2.0 * math.pi * f / 34.0) * math.pi / 32768.0
                dy = (wy(f, 1.0) - wy(f, 0.0))
                dlat = 0.3 * dp + 0.4 * math.atan(dy / 200.0) + math.radians(err_deg if f >= 60 else 0.0)
                e = [float(c[k]) for k in (5, 6, 7)]
                ce = [float(c[k]) for k in (8, 9, 10)]
                v = [e[k] - ce[k] for k in range(3)]
                h, R = math.hypot(v[0], v[2]), math.dist(e, ce)
                th = math.atan2(v[1], h) + dlat
                psi = math.atan2(v[0], v[2])
                c[5], c[6], c[7] = "%.3f" % (ce[0] + R * math.cos(th) * math.sin(psi)), "%.3f" % (ce[1] + R * math.sin(th)), "%.3f" % (ce[2] + R * math.cos(th) * math.cos(psi))
            return rec
        for name, kw, extra, bad in (
                ("unconverted ride camera on the swell, the ship's pitch / float y read by its latitude (talk10 930 / talk13 965 class)", dict(), [], set()),
                ("the same without the attitude term (--tol ship_att=0)", dict(), ["--tol", "ship_att=0"], {"cam-tslack"}),
                ("the same plus a real 1.5-degree latitude error from frame 60", dict(err_deg=1.5), [], {"cam-tslack"}),
                ("the same latitude gap with B's camera converted (no attitude term)", dict(conv=True), [], {"cam-tslack"})):
            B = os.path.join(tmp, "batt%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2, ship=SEAB, ride=(136, None, None, (1200.0, 36, 0.0, 0)))
            ship_pitch(B, 2, 35.0, 34.0)
            post_cam(B, 2, rec=att(2, kw.get("err_deg", 0.0)), unconv=not kw.get("conv"))
            rc, gates, out = cmp(AATT, B, *extra)
            got = {g for g in ("link-tslack", "cam-tslack") if gates.get(g) == "FAIL"}
            if got != bad or gates.get("ship") != "PASS":
                fails.append("%s: failed slack gates %s (want %s), ship %s\n%s" % (name, sorted(got), sorted(bad), gates.get("ship"), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # (3) sv2 135: the measured sweep and latch (frames 84-92 at test frames 30-38), held to the end: passes with the
        # held-direction term only; a held offset beyond the latch window (1200 s16 > half the last move 1688 + cam / R),
        # a direction that keeps drifting after the latch (no hold) and --tol cam_hold=0 fail
        AH = os.path.join(tmp, "a30hold")
        write_run(AH, 1)
        post_cam(AH, 1, rec=sv2_cam(SV2_A, 30))
        for name, kw, extra, bad in (
                ("sv2's held event-camera direction latched within the window (88 s16, 2.5 units for the hold)", dict(), [], set()),
                # (held_gap=0 too: sv2's radius grows 190 -> 300 while the 0.37-0.48 deg gap holds, which the held camera-gap rule (6m)'s
                # held-gap angle allowance also accepts: 0.476 deg at R 300, held 0.369 deg at the smaller radius)
                ("the same without the held-direction term (--tol cam_hold=0, held_gap=0)", dict(), ["--tol", "cam_hold=0", "--tol", "held_gap=0"], {"cam-tslack"}),
                ("the same without the held-direction term, 6m on (--tol cam_hold=0: the held-gap angle allowance takes it)", dict(), ["--tol", "cam_hold=0"], set()),
                ("a held offset beyond the latch window (B 1200 s16 further)", dict(held_err=-1200.0), [], {"cam-tslack"}),
                ("B's direction drifting 6 s16 a frame after the latch (no hold: a real lag)", dict(drift=-6.0), [], {"cam-tslack"})):
            B = os.path.join(tmp, "bhold%d" % len(bnames))
            bnames[name] = B
            write_run(B, 2)
            post_cam(B, 2, rec=sv2_cam(SV2_B, 30, **kw))
            rc, gates, out = cmp(AH, B, *extra)
            got = {g for g in ("cam-tslack",) if gates.get(g) == "FAIL"}
            if got != bad:
                fails.append("%s: failed slack gates %s (want %s)\n%s" % (name, sorted(got), sorted(bad), out))
            else:
                print("ok   %s%s" % (name, " (fails %s as it should)" % ", ".join(sorted(bad)) if bad else ""))
            if os.environ.get("C60_DEBUG"):
                print(out)
        # pair60.sh wiring: a stand-in run_scenario.sh writes a synthetic run from the step switch it is given
        fake = os.path.join(tmp, "fake_runner.sh")
        open(fake, "w").write('#!/bin/zsh\n# EXE GAME SAVE OUT END VAR=value...\nout=$4; shift 5\nn=1\n'
                              'for kv in "$@"; do case $kv in WWHD_T60_STEPS=*) n=${kv#*=};; esac; done\n'
                              'exec python3 %s --fake-run $out $n\n' % os.path.abspath(__file__))
        for n, want in (("2", 0), ("4", 0)):
            r = subprocess.run(["zsh", os.path.join(HERE, "pair60.sh"), "/bin/true", tmp, tmp, os.path.join(tmp, "pair" + n), "3"],
                               env=dict(os.environ, RUNNER=fake, T60_STEPS=n, SERIAL="1"), capture_output=True, text=True)
            if r.returncode != want or "equivalent: yes" not in r.stdout or "WWHD_T60_STEPS=%s" % n not in r.stdout:
                fails.append("pair60.sh N=%s: rc %d\n%s%s" % (n, r.returncode, r.stdout, r.stderr))
            else:
                print("ok   pair60.sh N=%s (stand-in runner): equivalent" % n)
        r = subprocess.run(["zsh", os.path.join(HERE, "pair60.sh"), "/bin/true", tmp, tmp, os.path.join(tmp, "pairx"), "3"],
                           env=dict(os.environ, RUNNER=fake, T60_ENV_B="WWHD_T60_STEPS=1", SERIAL="1"), capture_output=True, text=True)
        if r.returncode != 1 or "mode        FAIL" not in r.stdout:
            fails.append("pair60.sh with B at 30 Hz: rc %d, want 1 and a mode failure\n%s" % (r.returncode, r.stdout))
        else:
            print("ok   pair60.sh with B at 30 Hz fails the mode gate")
        rc, _, out = cmp(A, os.path.join(tmp, "nonexistent"))
        if rc != 2:
            fails.append("no origin: rc %d, want 2" % rc)
        else:
            print("ok   a run without origin exits 2")
    finally:
        shutil.rmtree(tmp, ignore_errors=True)  # this test's own folders
    for f in fails:
        print("FAIL", f)
    print("compare60_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
