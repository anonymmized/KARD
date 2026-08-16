# 0001. Own terminal renderer instead of FTXUI

## Status

Accepted, 2026-08-16

## Context

The input -> brain -> output loop works. After #22 there is a working TerminalSetup / TerminalInput / TerminalOutput triple, but no buffer: nothing remembers what was printed, so there is no scrollback, no resize handling and no line wrapping.

## Desicion

Write my own renderer: a buffer of logical lines + viewport + render().

## Consequences

At later stages it will be easier to implement and refactor without FTXUI taking over the main loop. On the other hand, the work itself gets harder: SIGWINCH, wrapping by display columns and event handling are all on me.
