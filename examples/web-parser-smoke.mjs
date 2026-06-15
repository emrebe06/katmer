import { parseKatmerDocument } from "../bindings/web/katmer-parser.js";

const parsed = parseKatmerDocument(`
<katmer module="http" type="str">
POST /checkout
Content-Type: application/json

{"product_id": 42}
</katmer>
`);

console.log(JSON.stringify(parsed, null, 2));
