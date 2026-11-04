# Next engineering steps

1. Integrate the official Neural Amp Modeler core/plugin DSP at a pinned, reviewed revision.
2. Load and validate `.nam` models off the audio thread; publish a ready DSP instance safely to the audio callback.
3. Add parameter attachments and restore editor controls from saved state.
4. Add automated audio smoke tests for mono/stereo, silence, finite output, bypass, and sample-rate changes.
5. Run CI on the target repository and inspect the actual VST3 artifact before release.

Do not claim the model is active until the DSP integration and model-load tests pass.
