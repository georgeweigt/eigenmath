module.exports = {
    "env": {
        "browser": true,
        "es2021": true
    },
    "extends": "eslint:recommended",
    "parserOptions": {
        "ecmaVersion": 12
    },
    "rules": {
        "no-unused-vars": ["error", { "varsIgnorePattern": "^run$" }]
    },
    "globals": {
        "fputs": "readonly"
    }
};
