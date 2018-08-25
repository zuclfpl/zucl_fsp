
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

--The IEEE.std_logic_unsigned contains definitions that allow 

  entity PR_WRP is
  port (
    wrp_out : out std_logic_vector(215 downto 0);
    clk : in std_logic;
      wrp_in : in std_logic_vector(215 downto 0)
      );

  end PR_WRP;
  
architecture Behavioral of PR_WRP is


--component



  
signal xor_port: std_logic_vector (215 downto 98);
attribute MARK_DEBUG: string;
attribute MARK_DEBUG of   xor_port :  signal is "TRUE";


 
begin
xor_port(98) <= '1' xor wrp_in (98);
xor_loop: for i in 99 to 215 generate
xor_port(i) <= xor_port(i-1) xor wrp_in (i);
end generate;


PR_Kernel: algoritmo_KNP
port map (

ap_clk => clk,

ap_rst_n  =>	wrp_in(97)          , 

m_axi_gmem_RDATA    	   =>	wrp_in (96 downto 65),
s_axi_control_WDATA        =>    wrp_in (64 downto 33), 
 

s_axi_control_AWADDR 	   =>	wrp_in (32 downto 25), 
m_axi_gmem_ARREADY   	   =>	wrp_in(24)          , 
m_axi_gmem_RVALID    	   =>	wrp_in (23)         , 
m_axi_gmem_RLAST     	   =>	wrp_in (22)         , 
--m_axi_gmem_RID       	   =>	wrp_in (26 downto 26),
m_axi_gmem_RID       	   =>	(others=>'0'),
 
--m_axi_gmem_RUSER     	   =>	wrp_in (25 downto 25), 
m_axi_gmem_RUSER     	   =>   (others=>'0'),
m_axi_gmem_RRESP     	   =>	wrp_in (21 downto 20), 
m_axi_gmem_BVALID    	   =>	wrp_in (19)         , 
--m_axi_gmem_BRESP     	   =>	wrp_in (21 downto 20), 
m_axi_gmem_BRESP     	   =>	(others=>'0'),
--m_axi_gmem_BID       	   =>	wrp_in (19 downto 19), 
m_axi_gmem_BID       	   =>	(others=>'0'),
--m_axi_gmem_BUSER      	   =>	wrp_in (18 downto 18),
m_axi_gmem_BUSER      	   =>	(others=>'0'),
s_axi_control_AWVALID 	   =>	wrp_in (18)         , 
s_axi_control_WVALID  	   =>	wrp_in (17)         , 
s_axi_control_WSTRB   	   =>	wrp_in (16 downto 13), 
s_axi_control_ARVALID 	   =>	wrp_in (12)         , 
s_axi_control_ARADDR  	   =>	wrp_in (11 downto 4), 
s_axi_control_RREADY  	   =>	wrp_in (3)          , 
s_axi_control_BREADY  	   =>	wrp_in (2)          , 
m_axi_gmem_AWREADY    	   =>	wrp_in (1)          , 
m_axi_gmem_WREADY     	   =>	wrp_in(0)           , 



--wrp_out (215 downto 214) => open,


m_axi_gmem_ARADDR          =>    wrp_out (215 downto 152), 
m_axi_gmem_AWADDR          =>    wrp_out (151 downto 88), 
s_axi_control_RDATA        =>    wrp_out (87 downto 56), 
m_axi_gmem_WDATA            =>    wrp_out (55 downto 24),                 
--m_axi_gmem_AWCACHE     	   =>	wrp_out (85 downto 82), 
m_axi_gmem_AWLEN (7 downto 4)  =>	open, 
m_axi_gmem_AWLEN (3 downto 0) =>	wrp_out (23 downto 20), 
--m_axi_gmem_AWPROT      	   =>	wrp_out (73 downto 71), 
--m_axi_gmem_AWQOS       	   =>	wrp_out (70 downto 67), 
--m_axi_gmem_AWREGION    	   =>	wrp_out (66 downto 63), 
--m_axi_gmem_AWUSER      	   =>	wrp_out (62 DOWNTO 62), 
m_axi_gmem_WSTRB       	   =>	wrp_out (19 downto 16), 
m_axi_gmem_WLAST       	   =>	wrp_out (15)          , 
--m_axi_gmem_WID         	   =>	wrp_out (56 downto 56), 
--m_axi_gmem_WUSER       	   =>	wrp_out (55 downto 55), 
m_axi_gmem_ARVALID     	   =>	wrp_out (14)          , 
--m_axi_gmem_ARID        	   =>	wrp_out (53 downto 53), 
m_axi_gmem_ARLEN (7 downto 4)  =>	open, 
m_axi_gmem_ARLEN (3 downto 0)      	   =>	wrp_out (13 downto 10), 
--m_axi_gmem_ARSIZE      	   =>	wrp_out (44 downto 42), 
--m_axi_gmem_ARBURST     	   =>	wrp_out (41 downto 40), 
--m_axi_gmem_ARLOCK      	   =>	wrp_out (39 downto 38), 
--m_axi_gmem_ARCACHE     	   =>	wrp_out (37 downto 34), 
--m_axi_gmem_ARPROT      	   =>	wrp_out (33 downto 31), 
--m_axi_gmem_ARQOS       	   =>	wrp_out (30 downto 27), 
--m_axi_gmem_ARREGION    	   =>	wrp_out (26 downto 23), 
--m_axi_gmem_ARUSER      	   =>	wrp_out (22 downto 22), 
m_axi_gmem_RREADY      	   =>	wrp_out (9)          , 
m_axi_gmem_BREADY      	   =>	wrp_out (8)          , 
s_axi_control_AWREADY  	   =>	wrp_out (7)          , 
s_axi_control_WREADY   	   =>	wrp_out (6)          , 
s_axi_control_ARREADY  	   =>	wrp_out (5)          , 
s_axi_control_RVALID   	   =>	wrp_out (4)          , 
--s_axi_control_RRESP    	   =>	wrp_out (15 downto 14), 
s_axi_control_BVALID   	   =>	wrp_out (3)          , 
--s_axi_control_BRESP    	   =>	wrp_out (12 DOWNTO 11), 
m_axi_gmem_WVALID      	   =>	wrp_out (2)          , 
m_axi_gmem_AWVALID     	   =>	wrp_out (1)           , 
--m_axi_gmem_AWID        	   =>	wrp_out (8 DOWNTO 8)  , 
--m_axi_gmem_AWSIZE      	   =>	wrp_out (7 downto 5)  , 
--m_axi_gmem_AWBURST     	   =>	wrp_out (4 downto 3)  , 
--m_axi_gmem_AWLOCK      	   =>	wrp_out (2 downto 1)  , 

interrupt              	   =>	wrp_out (0)             

);  	


end Behavioral;
