"""Time-budgeted async task runner for chunk generation."""
from __future__ import annotations

import time
from collections import deque
from collections.abc import Callable

Task = Callable[[], None]


class FrameBudgetExecutor:
    def __init__(self, budget_ms: float = 10.0):
        self.budget_ms = budget_ms
        self._queue: deque[Task] = deque()

    def schedule(self, task: Task) -> None:
        self._queue.append(task)

    def update(self) -> int:
        start = time.perf_counter()
        executed = 0
        while self._queue:
            elapsed_ms = (time.perf_counter() - start) * 1000.0
            if elapsed_ms >= self.budget_ms:
                break
            self._queue.popleft()()
            executed += 1
        return executed

    @property
    def pending(self) -> int:
        return len(self._queue)
