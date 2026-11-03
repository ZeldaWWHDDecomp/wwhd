# AGL#18 companion sources

These are project-owned verification fixtures for AGL#18. They preserve exact hand-written mutation or exact generated-candidate sources copied from this unit's local evidence so the companion specs remain reproducible from a clean checkout.

The `empty_hook_*` and `geometry_old_store_control.cpp` files are owned hand variants; the `binding_survivor_*_mutant3.cpp` files preserve the exact candidates selected by this unit's mutation campaign.

The original `027CF82C` `prepareSelectedBinding` body was also checked against
`027CC160`: their instruction streams are semantically and relocation-normalized
identical. They are not literally byte-identical because three relative branch
instructions have different encodings at the two function addresses.
