# JSUList

Full local inventory: ten entries027EC50C..027EC907. Eight mapped methods are supplemented by the unnamed JSUPtrList destructor027EC68C and the adjacent SDK initializer027EC8E0. The initializer's TU attribution remains inferred from adjacency; previous generated entries027EC4F4/027EC508 and the next output-stream entry027EC908 are excluded.

The native list is12bytes (head+0,tail+4,unsigned count+8); a link is16bytes (object+0,list+4,previous+8,next+12). The HD link constructor explicitly allocates16bytes when incoming this is null. Destructors use the low deletion bit; list destruction clears each traversed link's parent and reloads the unsigned count after that store. Append/prepend/insert preserve the native remove-call result rather than reducing a nonzero result to1. Source store/reload ordering follows HD, including head/tail updates and count reloads, so guest-address alias cases remain testable. The GC logical API is retained without importing host-pointer layouts or assuming GC compiler inlining.

All ten functions pass10000generated inputs at seeds1and7 without aborts/crashes/timeouts. Coverage55/55blocks at both seeds, no remaining gaps. The bounded17-case mutation sample changes real operators/constants across every root: all17compiled and were detected by returning observation differences; zero survivors/invalid cases. The destructor uses finite zero/one/two/three-node fixtures; other argument steering retains the unsteered eighth for address/alias stress. No stable-region exemption, indirect-call exemption or shared harness change is used.

History: the first1000-input probe exposed a mistyped SDK initializer address10492470; the native address104A2470 was corrected before the final gates. The initially missing head-removal block was reached with explicit list/link argument steering. These earlier checkpoints receive no final gate credit.

