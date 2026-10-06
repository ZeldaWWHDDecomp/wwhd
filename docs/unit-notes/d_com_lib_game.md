# d_com_lib_game verified

Full native inventory: 02525FE4..02526044 is the 96-byte common game phase dispatcher; 02526044..025260D8 is the 148-byte adjacent per-TU SDK initializer. The trailing initializer attribution is inferred from per-TU ordering, not a recovered symbol. Previous 02525F50 belongs to the preceding d_com_inf_game TU; following 025260D8 starts the next subsystem. No claim on neighboring functions.

The HD dispatcher preserves all three input pointers across calls, repeating cPhs_Handler (0201A0AC) while its return is 2. Unlike the GC recursive source it is reconstructed as an equivalent loop. The initializer preserves native global writes, float loads/stores and five SDK calls in order. It uses the actual HD guest addresses. No local guest objects, indirect calls or nolivein exemptions occur.

Final unchanged source: both functions pass 10,000 generated inputs at seeds 1 and 7. Coverage is 3/3 and 1/1 respectively, union 4/4; no gaps. Fixture steers cPhs_Handler returns 0..5 to cover loop continuation and exit. Callees remain mocked under the standard isolated-function contract; this does not certify their internal behavior.

Bounded mutation closure: deterministic seed-7 sample of 24 plus one explicit loop-condition inversion, 25 total attempts. Seven compiled valid mutations produce completed returning differences in 1,000 generated inputs: both loop-condition mutations, three global write-address changes, a float load-address change, and SDK constructor argument-address change. No valid survivors. Four mutations replacing calls with unaligned nonexistent entries are invalid-target diagnostics, excluded; fourteen compile failures excluded. No crash/timeout detections. Initial stock-tool summary called all ten compiled sample entries detected; its four invented-target counts were explicitly corrected in mutation-closure.json. The extra loop inversion demonstrates control-flow sensitivity independently of literal perturbations.

