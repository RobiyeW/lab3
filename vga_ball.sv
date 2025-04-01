/*
 * Avalon memory-mapped peripheral that generates VGA with a bouncing ball
 *
 * Stephen A. Edwards
 * Columbia University
 *
 * Register map:
 * 
 * Byte Offset  7 ... 0   Meaning
 *        0    |  X Low  | Lower 8 bits of ball X coordinate
 *        1    |  X High | Higher 8 bits of ball X coordinate
 *        2    |  Y Low  | Lower 8 bits of ball Y coordinate
 *        3    |  Y High | Higher 8 bits of ball Y coordinate
 *        4    |  Red    | Red component of background color (0-255)
 *        5    |  Green  | Green component
 *        6    |  Blue   | Blue component
 */

module vga_ball (
    input logic clk,
    input logic reset,
    input logic [7:0] writedata,
    input logic write,
    input logic chipselect,
    input logic [2:0] address,

    output logic [7:0] VGA_R, VGA_G, VGA_B,
    output logic VGA_CLK, VGA_HS, VGA_VS,
    output logic VGA_BLANK_n,
    output logic VGA_SYNC_n
);

   logic [10:0] hcount;
   logic [9:0] vcount;

   logic [7:0] background_r, background_g, background_b;
   //logic [9:0] ball_x, ball_y; // Ball coordinates (10 bits)

   // New coordinate tracking
   logic [9:0] current_x, current_y;  // Active coordinates
   logic [9:0] shadow_x, shadow_y;     // Buffered coordinates
   logic update_pending;               // Update flag


   vga_counters counters(.clk50(clk), .*);

   // Registers for background color and ball coordinates
   always_ff @(posedge clk) begin
     if (reset) begin
        background_r <= 8'h00;
        background_g <= 8'h00;
        background_b <= 8'h80;
        
        // ball_x <= 10'd320;
        // ball_y <= 10'd240;

        //new coordinates initializing
        shadow_x <= 10'd320;
        shadow_y <= 10'd240;
        current_x <= 10'd320;
        current_y <= 10'd240;
        update_pending <= 1'b0;
     end else if (chipselect && write) begin
        case (address)
          3'h0 : ball_x[7:0] <= writedata;          // X Low
          3'h1 : ball_x[9:8] <= writedata[1:0];     // X High
          3'h2 : ball_y[7:0] <= writedata;          // Y Low
          3'h3 : ball_y[9:8] <= writedata[1:0];     // Y High
          3'h4 : background_r <= writedata;         // Red
          3'h5 : background_g <= writedata;         // Green
          3'h6 : background_b <= writedata;         // Blue
        endcase
        if (address <= 3'h3) update_pending <= 1'b1;
     end
     // New updating active registers duirng vertical blanking
     if (vcount >= 10'd480 && update_pending) begin
        current_x <= shadow_x;
        current_y <= shadow_y;
        update_pending <= 1'b0;
      end
   end

   // Draw Ball and Background Logic
   always_comb begin
      {VGA_R, VGA_G, VGA_B} = {8'h0, 8'h0, 8'h0};
      if (VGA_BLANK_n) begin
         if ((hcount >= current_x) && (hcount < current_x + 16) &&
             (vcount >= current_y) && (vcount < current_y + 16)) begin
            {VGA_R, VGA_G, VGA_B} = {8'hff, 8'hff, 8'hff};
         end else begin
            {VGA_R, VGA_G, VGA_B} = {background_r, background_g, background_b}; // Background
         end
      end
   end

endmodule

module vga_counters(
    input logic clk50, reset,
    output logic [10:0] hcount,
    output logic [9:0] vcount,
    output logic VGA_CLK, VGA_HS, VGA_VS, VGA_BLANK_n, VGA_SYNC_n
);

   parameter HACTIVE      = 11'd 1280;
   parameter HFRONT_PORCH = 11'd 32;
   parameter HSYNC        = 11'd 192;
   parameter HBACK_PORCH  = 11'd 96;
   parameter HTOTAL       = HACTIVE + HFRONT_PORCH + HSYNC + HBACK_PORCH;

   parameter VACTIVE      = 10'd 480;
   parameter VFRONT_PORCH = 10'd 10;
   parameter VSYNC        = 10'd 2;
   parameter VBACK_PORCH  = 10'd 33;
   parameter VTOTAL       = VACTIVE + VFRONT_PORCH + VSYNC + VBACK_PORCH;

   logic endOfLine;

   always_ff @(posedge clk50 or posedge reset)
     if (reset) hcount <= 0;
     else if (endOfLine) hcount <= 0;
     else hcount <= hcount + 11'd1;

   assign endOfLine = (hcount == HTOTAL - 1);

   logic endOfField;

   always_ff @(posedge clk50 or posedge reset)
     if (reset) vcount <= 0;
     else if (endOfLine) begin
       if (endOfField) vcount <= 0;
       else vcount <= vcount + 10'd1;
     end

   assign endOfField = (vcount == VTOTAL - 1);

   assign VGA_HS = !( (hcount >= HACTIVE + HFRONT_PORCH) &&
                       (hcount < HACTIVE + HFRONT_PORCH + HSYNC) );

   assign VGA_VS = !( (vcount >= VACTIVE + VFRONT_PORCH) &&
                       (vcount < VACTIVE + VFRONT_PORCH + VSYNC) );

   assign VGA_BLANK_n = (hcount < HACTIVE) && (vcount < VACTIVE);
   assign VGA_SYNC_n = 1'b0;
   assign VGA_CLK = clk50;

endmodule
