
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

--The IEEE.std_logic_unsigned contains definitions that allow 
--std_logic_vector types to be used with the + operator to instantiate a 
--counter.
use IEEE.std_logic_unsigned.all;

entity MOD_TOP is
    Port (     
    clk : in std_logic;
I : in  std_logic;
O : out std_logic
			  );
end MOD_TOP;

architecture Behavioral of MOD_TOP is
---- GoAhead
---- attribute_declaration

-- signal_declaration
signal s2p : std_logic_vector(215 downto 0);
signal p2s : std_logic_vector(215 downto 0);


-- attribute_assignment
attribute s : string;
attribute keep : string;
attribute s of s2p : signal is "true";
attribute s of p2s : signal is "true";
attribute keep of s2p : signal is "true";
attribute keep of p2s : signal is "true";

-- component_declaration
component StaticPlaceholder is
    Port (  
            NET_Left_X1Y5_out    : out std_logic_vector(215 downto 0);
            NET_Left_X1Y5_in     : in std_logic_vector(215 downto 0);
            NET_Right_X1Y5_out   : out std_logic_vector(215 downto 0);
            NET_Right_X1Y5_in    : in std_logic_vector(215 downto 0));

    
   
end component StaticPlaceholder;


component PR_WRP_0 is
port (
    wrp_out : out std_logic_vector(215 downto 0);
        clk : in std_logic;
    wrp_in : in std_logic_vector(215 downto 0)
    );

end component PR_WRP_0;
  
begin

inst_StaticPlaceholder: StaticPlaceholder
port map (


        NET_Left_X1Y5_in (215 downto 0) => p2s,
        NET_Left_X1Y5_out (215 downto 0) => s2p,    
        NET_Right_X1Y5_in (215 downto 0) => (others=>'0'),
        NET_Right_X1Y5_out (215 downto 0) => open    
	

);

inst_PR_WRP_0: PR_WRP_0
port map (
	wrp_in => s2p,
	clk => clk,
	wrp_out  => p2s
);




process (clk)
begin
if (rising_edge(clk)) then
O <= I;
end if;
end process;





end Behavioral;
