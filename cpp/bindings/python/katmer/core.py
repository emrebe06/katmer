from __future__ import annotations

import ctypes
import json
import os
from pathlib import Path
from typing import Any


class KatmerError(RuntimeError):
    pass


def _default_library_name() -> str:
    if os.name == "nt":
        return "katmer_core.dll"
    if os.uname().sysname == "Darwin":  # type: ignore[attr-defined]
        return "libkatmer_core.dylib"
    return "libkatmer_core.so"


class Katmer:
    def __init__(self, library: str | os.PathLike[str] | None = None):
        library_path = Path(library or os.environ.get("KATMER_NATIVE_LIBRARY", _default_library_name()))
        self.library_path = str(library_path.expanduser())
        self.lib = ctypes.CDLL(self.library_path)
        self._configure()

    def bake(self, value: str | bytes, input_type: str = "katmer") -> dict[str, Any]:
        raw = value if isinstance(value, bytes) else value.encode("utf-8")
        ptr = self.lib.katmer_abi_bake_json(input_type.encode("utf-8"), raw, len(raw))
        if not ptr:
            raise KatmerError("katmer_abi_bake_json returned null")
        try:
            text = ctypes.cast(ptr, ctypes.c_char_p).value.decode("utf-8")
            return json.loads(text)
        finally:
            self.lib.katmer_free_string(ptr)

    def bake_http(self, payload: str, *, kind: str = "request") -> dict[str, Any]:
        return self.bake(f'<katmer module="http" type="{kind}">{payload}</katmer>')

    def bake_dsp(self, payload: str, *, kind: str = "dsp.pipeline") -> dict[str, Any]:
        return self.bake(f'<katmer module="audio" type="{kind}">{payload}</katmer>')

    def modules(self) -> list[dict[str, Any]]:
        ptr = self.lib.katmer_modules_json()
        if not ptr:
            return []
        try:
            text = ctypes.cast(ptr, ctypes.c_char_p).value.decode("utf-8")
            return json.loads(text)
        finally:
            self.lib.katmer_free_string(ptr)

    def abi(self) -> dict[str, Any]:
        ptr = self.lib.katmer_abi_info_json()
        if not ptr:
            return {}
        try:
            text = ctypes.cast(ptr, ctypes.c_char_p).value.decode("utf-8")
            return json.loads(text)
        finally:
            self.lib.katmer_free_string(ptr)

    def _configure(self) -> None:
        self.lib.katmer_abi_bake_json.argtypes = [ctypes.c_char_p, ctypes.c_void_p, ctypes.c_size_t]
        self.lib.katmer_abi_bake_json.restype = ctypes.c_void_p
        self.lib.katmer_modules_json.argtypes = []
        self.lib.katmer_modules_json.restype = ctypes.c_void_p
        self.lib.katmer_abi_info_json.argtypes = []
        self.lib.katmer_abi_info_json.restype = ctypes.c_void_p
        self.lib.katmer_free_string.argtypes = [ctypes.c_void_p]
        self.lib.katmer_free_string.restype = None
