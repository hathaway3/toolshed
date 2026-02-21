---
trigger: always_on
---

# Local & Remote LLM Routing Rules

## 1. Primary Planner (Cloud)
- **Model:** Gemini 3 (Cloud)
- **Usage:** Use for high-level architectural decisions, cross-file dependency mapping, and complex Rust refactoring.

## 2. Executioner: Local GPU (RTX 5070)
- **Provider:** `ollama-remote` (Gemma 3)
- **Primary Tasks:**
    - **Rust Logic:** Writing function bodies, unit tests, and routine boilerplate.
    - **Documentation:** Generating JSDoc, Docstrings, and README updates.
    - **Low Latency:** Use for small, iterative edits where fast feedback is critical.

## 3. Worker: Remote Server (192.168.100.194)
- **Provider:** `ollama-remote` (Gemma 3)
- **Primary Tasks:**
    - **Heavy Context:** Analyzing large logs or long context files (e.g., full crate indexing).
    - **Batch Operations:** Generating extensive test suites or documentation for entire modules.
    - **Background Tasks:** Use for non-urgent execution to keep local VRAM free for the IDE UI.

## 4. Operational Constraints
- **Context Privacy:** Local code indexing and embeddings must stay on-device or within the local network (.194).
- **Fallback:** If `ollama-local` is unresponsive, fall back to `ollama-remote` before escalating to Cloud.