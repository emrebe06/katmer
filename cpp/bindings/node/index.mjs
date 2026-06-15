import { spawnSync } from "node:child_process";
import { existsSync } from "node:fs";
import path from "node:path";

function defaultBakeExecutable() {
  if (process.env.KATMER_BAKE_EXE) return process.env.KATMER_BAKE_EXE;
  const suffix = process.platform === "win32" ? ".exe" : "";
  return path.resolve(process.cwd(), "build", "Release", `katmer_bake${suffix}`);
}

function parseKatmerDocument(source) {
  const text = String(source ?? "");
  const match = text.match(/<katmer([^>]*)>([\s\S]*?)<\/katmer>/i);
  if (!match) {
    return { ok: false, engine: "katmer-js-fallback", code: "PARSE_ERROR", message: "missing <katmer> document" };
  }
  const attrs = {};
  for (const attr of match[1].matchAll(/([a-zA-Z0-9_.:-]+)=["']([^"']*)["']/g)) attrs[attr[1]] = attr[2];
  return {
    ok: true,
    engine: "katmer-js-fallback",
    module: attrs.module || "core",
    type: attrs.type || "str",
    attrs,
    result: {
      module: attrs.module || "core",
      type: attrs.type || "str",
      body: match[2].trim(),
      select_kind: attrs.module === "audio" || attrs.module === "dsp" ? "dsp" : attrs.module || "core"
    }
  };
}

export class Katmer {
  constructor(options = {}) {
    this.executable = options.executable || defaultBakeExecutable();
    this.preferNative = options.preferNative ?? true;
  }

  bake(source, inputType = "katmer") {
    if (this.preferNative && existsSync(this.executable)) {
      const child = spawnSync(this.executable, [inputType], {
        input: String(source ?? ""),
        encoding: "utf8",
        windowsHide: true
      });
      if (child.status === 0 && child.stdout.trim()) {
        return JSON.parse(child.stdout);
      }
      if (child.error) throw child.error;
      throw new Error(child.stderr || `katmer_bake exited with ${child.status}`);
    }
    return parseKatmerDocument(source);
  }

  bakeHttp(payload, type = "request") {
    return this.bake(`<katmer module="http" type="${type}">${payload}</katmer>`);
  }

  bakeDsp(payload, type = "dsp.pipeline") {
    return this.bake(`<katmer module="audio" type="${type}">${payload}</katmer>`);
  }
}

export { parseKatmerDocument };
