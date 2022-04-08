/** @type {import('ts-jest/dist/types').InitialOptionsTsJest} */
module.exports = {
  preset: "ts-jest",
  testEnvironment: "node",
  modulePathIgnorePatterns: ["build"],
  globals: {
    "ts-jest": {
      isolatedModules: true,
    },
  },
  slowTestThreshold: 1
};
