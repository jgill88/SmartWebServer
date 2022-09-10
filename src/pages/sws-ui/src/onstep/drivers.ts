import { Axis } from "../types";
import { Queryable } from "./queryable";

export class AxisDriver implements Queryable {
  private failedQueries = 0;
  private _status?: Axis;
  constructor(private index: number) {}

  get status() {
    return this._status;
  }

  get queryCommand() {
    if (this.disabled) {
      return null;
    }

    return `:GXU${this.index + 1}#`;
  }

  private get axisName() {
    switch (this.index) {
      case 0:
        return "ra";
      case 1:
        return "dec";
    }
    return this.index + 1;
  }

  get disabled() {
    return this.failedQueries > 3;
  }

  handleResponse(responses: string[]) {
    const response = responses[this.index];
    if (typeof response === "undefined") {
      return;
    }

    const test = (v: string) => response.indexOf(v) > -1;

    if (response === "0") {
      this.failedQueries++;
      this._status = {
        valid: false,
      };
      return;
    }

    this._status = {
      valid: true,
      name: this.axisName,
      commsFailure: test("ST,OA,OB,GA,GB,OT,PW"),
      standStill: test("ST"),
      outputA: {
        open: test("OA"),
        short: test("GA"),
      },
      outputB: {
        open: test("OB"),
        short: test("GB"),
      },
      overTemp: test("OT"),
      almostOverTemp: test("PW"),
      fault: test("GF"),
    };

    return response;
  }
}
