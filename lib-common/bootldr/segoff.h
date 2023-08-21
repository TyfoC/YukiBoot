#pragma once
#ifndef SEGOFF_H
#define SEGOFF_H

#define SEGOFF_TO_PHYS($seg, $off)			((size_t)(((($seg) & 0xFFFF) << 4) + (($off) & 0xFFFF)))

#endif