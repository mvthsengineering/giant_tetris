const uint8_t bar0[4] =
{
	B00000000,
	B00000000,
	B00111000,
	B00000000,
};
const uint8_t bar1[4] =
{
	B00000000,
	B00010000,
	B00010000,
	B00010000,
};

const uint8_t ell0[4] =
{
	B00000000,
	B00000000,
	B00011000,
	B00010000,
};
const uint8_t ell1[4] =
{
	B00000000,
	B00000000,
	B00110000,
	B00010000,
};
const uint8_t ell2[4] =
{
	B00000000,
	B00010000,
	B00110000,
	B00000000,
};
const uint8_t ell3[4] =
{
	B00000000,
	B00010000,
	B00011000,
	B00000000,
};

const uint8_t tee0[4] =
{
	B00000000,
	B00000000,
	B00111000,
	B00010000,
};
const uint8_t tee1[4] =
{
	B00000000,
	B00010000,
	B00110000,
	B00010000,
};
const uint8_t tee2[4] =
{
	B00000000,
	B00010000,
	B00111000,
	B00000000,
};
const uint8_t tee3[4] =
{
	B00000000,
	B00010000,
	B00011000,
	B00010000,
};


const uint8_t blk0[4] =
{
	B00000000,
	B00000000,
	B00011000,
	B00011000,
};

const uint8_t * pieces[11] 	   = { bar0, bar1, ell0, ell1, ell2, ell3, tee0, tee1, tee2, tee3, blk0 };
const uint8_t * piecesRotated[11]  = { bar1, bar0, ell1, ell2, ell3, ell0, tee1, tee2, tee3, tee0, blk0 };
const uint8_t * piecesGenerated[4] = { bar0, ell0, tee0, blk0 };
