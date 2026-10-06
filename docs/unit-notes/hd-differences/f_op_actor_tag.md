### f_op_actor_tag — actor queue tags

The HD unit retains the three GameCube queue-tag operations: append a tag to the global actor queue, remove a tag through the shared tree-cut helper, and initialize a tag with its actor data pointer before returning success. HD stores its global actor queue at a fixed address and emits the append/remove wrappers as tail calls. Their return values and incoming argument registers are preserved.

HD additionally emits a per-unit static initializer for SDK registration and local math globals. The complete native unit has four entries at025DA2F8,025DA308,025DA30C,025DA330; the initializer ends025DA3C0. The preceding025DA2F4 empty destructor is already in the actor-manager unit, and the following025DA3C4 begins camera processing. The initializer association is inferred from this native adjacency.

The candidate performs the same direct calls and registration stores in native order. No layout modification, indirect call, local object passed to a callee, input restriction, or shared harness change is needed. Both seeds test the unrestricted generated reference contracts while queue and SDK callees remain mocked. This validates the wrappers, not a complete native queue lifecycle or gameplay integration.
