#pragma once
//All 1 based index
struct FEATURE{
	char* idstr;
	char  ciDataType;
	WORD  wiSerial;	//base index 1
	FEATURE(char* id_str=NULL,char ciType=0)
	{
		idstr=id_str;
		ciDataType=ciType;
	}
	void Init(WORD wIndexSerial,char ciType=0)
	{
		ciDataType=ciType;
		wiSerial=wIndexSerial;
	}
public:
/*   1 Feature's id string:*/static const char* DRAWING_WELD_BRIEFSTYLE;	//���졢���϶��缰���ݶ����ȳ��ҵĸְ��麸ͼģʽ���������弰����ͼֱ�ӻ��ڵװ��ϣ� wjh-2016.9.13
/*   2 Feature's id string:*/static const char* DRAWING_ANGLE_SPLIT2VIEWSTYLE;
/*   3 Feature's id string:*/static const char* LOCALIZE_CUSTOMIZE_CLIENT;
/*   4 Feature's id string:*/static const char* MODEL_MEASURE_SOLIDKEYPOINT;
/*   5 Feature's id string:*/static const char* MODEL_CHECK_SIZEnSPECINFO;
/*   6 Feature's id string:*/static const char* MODEL_CHECK_BOMCOMPARE;
/*   7 Feature's id string:*/static const char* BATCH_INTELLI_PRINT_PROCESSCARD;	//���պ�Դ���Ƶ�������ӡ�Ǹ�(Բ��)���տ�����
/*   8 Feature's id string:*/static const char* SAVEAS_FILE_VERSION;		//��������ľɸ�ʽ�����ļ��汾
/*   9 Feature's id string:*/static const char* EXPORT_3DSOLID_DWGMODEL;	//������άʵ��DWGģ��
/*  10 Feature's id string:*/
/*  11 Feature's id string:*/
/*  12 Feature's id string:*/
/*  13 Feature's id string:*/
/*  14 Feature's id string:*/
/*  15 Feature's id string:*/
/*  16 Feature's id string:*/
/*  17 Feature's id string:*/
/*  18 Feature's id string:*/
/*  19 Feature's id string:*/
/*  20 Feature's id string:*/
/*  21 Feature's id string:*/
/*  22 Feature's id string:*/
/*  23 Feature's id string:*/
/*  24 Feature's id string:*/
/*  25 Feature's id string:*/
/*  26 Feature's id string:*/
/*  27 Feature's id string:*/
/*  28 Feature's id string:*/
/*  29 Feature's id string:*/
/*  30 Feature's id string:*/
/*  31 Feature's id string:*/
/*  32 Feature's id string:*/
/*  33 Feature's id string:*/
/*  34 Feature's id string:*/
/*  35 Feature's id string:*/
/*  36 Feature's id string:*/
/*  37 Feature's id string:*/
/*  38 Feature's id string:*/
/*  39 Feature's id string:*/
/*  40 Feature's id string:*/
/*  41 Feature's id string:*/
/*  42 Feature's id string:*/
/*  43 Feature's id string:*/
/*  44 Feature's id string:*/
/*  45 Feature's id string:*/
/*  46 Feature's id string:*/
/*  47 Feature's id string:*/
/*  48 Feature's id string:*/
/*  49 Feature's id string:*/
/*  50 Feature's id string:*/
/*  51 Feature's id string:*/
/*  52 Feature's id string:*/
/*  53 Feature's id string:*/
/*  54 Feature's id string:*/
/*  55 Feature's id string:*/
/*  56 Feature's id string:*/
/*  57 Feature's id string:*/
/*  58 Feature's id string:*/
/*  59 Feature's id string:*/
/*  60 Feature's id string:*/
/*  61 Feature's id string:*/
/*  62 Feature's id string:*/
/*  63 Feature's id string:*/
/*  64 Feature's id string:*/
/*  65 Feature's id string:*/
/*  66 Feature's id string:*/
/*  67 Feature's id string:*/
/*  68 Feature's id string:*/
/*  69 Feature's id string:*/
/*  70 Feature's id string:*/
/*  71 Feature's id string:*/
/*  72 Feature's id string:*/
/*  73 Feature's id string:*/
/*  74 Feature's id string:*/
/*  75 Feature's id string:*/
/*  76 Feature's id string:*/
/*  77 Feature's id string:*/
/*  78 Feature's id string:*/
/*  79 Feature's id string:*/
/*  80 Feature's id string:*/
/*  81 Feature's id string:*/
/*  82 Feature's id string:*/
/*  83 Feature's id string:*/
/*  84 Feature's id string:*/
/*  85 Feature's id string:*/
/*  86 Feature's id string:*/
/*  87 Feature's id string:*/
/*  88 Feature's id string:*/
/*  89 Feature's id string:*/
/*  90 Feature's id string:*/
/*  91 Feature's id string:*/
/*  92 Feature's id string:*/
/*  93 Feature's id string:*/
/*  94 Feature's id string:*/
/*  95 Feature's id string:*/
/*  96 Feature's id string:*/
/*  97 Feature's id string:*/
/*  98 Feature's id string:*/
/*  99 Feature's id string:*/
/* 100 Feature's id string:*/
/* 101 Feature's id string:*/
/* 102 Feature's id string:*/
/* 103 Feature's id string:*/
/* 104 Feature's id string:*/
/* 105 Feature's id string:*/
/* 106 Feature's id string:*/
/* 107 Feature's id string:*/
/* 108 Feature's id string:*/
/* 109 Feature's id string:*/
/* 110 Feature's id string:*/
/* 111 Feature's id string:*/
/* 112 Feature's id string:*/
/* 113 Feature's id string:*/
/* 114 Feature's id string:*/
/* 115 Feature's id string:*/
/* 116 Feature's id string:*/
/* 117 Feature's id string:*/
/* 118 Feature's id string:*/
/* 119 Feature's id string:*/
/* 120 Feature's id string:*/
/* 121 Feature's id string:*/
/* 122 Feature's id string:*/
/* 123 Feature's id string:*/
/* 124 Feature's id string:*/
/* 125 Feature's id string:*/
/* 126 Feature's id string:*/
/* 127 Feature's id string:*/
/* 128 Feature's id string:*/
/* 129 Feature's id string:*/
/* 130 Feature's id string:*/
/* 131 Feature's id string:*/
/* 132 Feature's id string:*/
/* 133 Feature's id string:*/
/* 134 Feature's id string:*/
/* 135 Feature's id string:*/
/* 136 Feature's id string:*/
/* 137 Feature's id string:*/
/* 138 Feature's id string:*/
/* 139 Feature's id string:*/
/* 140 Feature's id string:*/
/* 141 Feature's id string:*/
/* 142 Feature's id string:*/
/* 143 Feature's id string:*/
/* 144 Feature's id string:*/
/* 145 Feature's id string:*/
/* 146 Feature's id string:*/
/* 147 Feature's id string:*/
/* 148 Feature's id string:*/
/* 149 Feature's id string:*/
/* 150 Feature's id string:*/
/* 151 Feature's id string:*/
/* 152 Feature's id string:*/
/* 153 Feature's id string:*/
/* 154 Feature's id string:*/
/* 155 Feature's id string:*/
/* 156 Feature's id string:*/
/* 157 Feature's id string:*/
/* 158 Feature's id string:*/
/* 159 Feature's id string:*/
/* 160 Feature's id string:*/
/* 161 Feature's id string:*/
/* 162 Feature's id string:*/
/* 163 Feature's id string:*/
/* 164 Feature's id string:*/
/* 165 Feature's id string:*/
/* 166 Feature's id string:*/
/* 167 Feature's id string:*/
/* 168 Feature's id string:*/
/* 169 Feature's id string:*/
/* 170 Feature's id string:*/
/* 171 Feature's id string:*/
/* 172 Feature's id string:*/
/* 173 Feature's id string:*/
/* 174 Feature's id string:*/
/* 175 Feature's id string:*/
/* 176 Feature's id string:*/
/* 177 Feature's id string:*/
/* 178 Feature's id string:*/
/* 179 Feature's id string:*/
/* 180 Feature's id string:*/
/* 181 Feature's id string:*/
/* 182 Feature's id string:*/
/* 183 Feature's id string:*/
/* 184 Feature's id string:*/
/* 185 Feature's id string:*/
/* 186 Feature's id string:*/
/* 187 Feature's id string:*/
/* 188 Feature's id string:*/
/* 189 Feature's id string:*/
/* 190 Feature's id string:*/
/* 191 Feature's id string:*/
/* 192 Feature's id string:*/
/* 193 Feature's id string:*/
/* 194 Feature's id string:*/
/* 195 Feature's id string:*/
/* 196 Feature's id string:*/
/* 197 Feature's id string:*/
/* 198 Feature's id string:*/
/* 199 Feature's id string:*/
/* 200 Feature's id string:*/
/* 201 Feature's id string:*/
/* 202 Feature's id string:*/
/* 203 Feature's id string:*/
/* 204 Feature's id string:*/
/* 205 Feature's id string:*/
/* 206 Feature's id string:*/
/* 207 Feature's id string:*/
/* 208 Feature's id string:*/
/* 209 Feature's id string:*/
/* 210 Feature's id string:*/
/* 211 Feature's id string:*/
/* 212 Feature's id string:*/
/* 213 Feature's id string:*/
/* 214 Feature's id string:*/
/* 215 Feature's id string:*/
/* 216 Feature's id string:*/
/* 217 Feature's id string:*/
/* 218 Feature's id string:*/
/* 219 Feature's id string:*/
/* 220 Feature's id string:*/
/* 221 Feature's id string:*/
/* 222 Feature's id string:*/
/* 223 Feature's id string:*/
/* 224 Feature's id string:*/
/* 225 Feature's id string:*/
/* 226 Feature's id string:*/
/* 227 Feature's id string:*/
/* 228 Feature's id string:*/
/* 229 Feature's id string:*/
/* 230 Feature's id string:*/
/* 231 Feature's id string:*/
/* 232 Feature's id string:*/
/* 233 Feature's id string:*/
/* 234 Feature's id string:*/
/* 235 Feature's id string:*/
/* 236 Feature's id string:*/
/* 237 Feature's id string:*/
/* 238 Feature's id string:*/
/* 239 Feature's id string:*/
/* 240 Feature's id string:*/
/* 241 Feature's id string:*/
/* 242 Feature's id string:*/
/* 243 Feature's id string:*/
/* 244 Feature's id string:*/
/* 245 Feature's id string:*/
/* 246 Feature's id string:*/
/* 247 Feature's id string:*/
/* 248 Feature's id string:*/
/* 249 Feature's id string:*/
/* 250 Feature's id string:*/
/* 251 Feature's id string:*/
/* 252 Feature's id string:*/
/* 253 Feature's id string:*/
/* 254 Feature's id string:*/
/* 255 Feature's id string:*/
/* 256 Feature's id string:*/
/* 257 Feature's id string:*/
/* 258 Feature's id string:*/
/* 259 Feature's id string:*/
/* 260 Feature's id string:*/
/* 261 Feature's id string:*/
/* 262 Feature's id string:*/
/* 263 Feature's id string:*/
/* 264 Feature's id string:*/
/* 265 Feature's id string:*/
/* 266 Feature's id string:*/
/* 267 Feature's id string:*/
/* 268 Feature's id string:*/
/* 269 Feature's id string:*/
/* 270 Feature's id string:*/
/* 271 Feature's id string:*/
/* 272 Feature's id string:*/
/* 273 Feature's id string:*/
/* 274 Feature's id string:*/
/* 275 Feature's id string:*/
/* 276 Feature's id string:*/
/* 277 Feature's id string:*/
/* 278 Feature's id string:*/
/* 279 Feature's id string:*/
/* 280 Feature's id string:*/
/* 281 Feature's id string:*/
/* 282 Feature's id string:*/
/* 283 Feature's id string:*/
/* 284 Feature's id string:*/
/* 285 Feature's id string:*/
/* 286 Feature's id string:*/
/* 287 Feature's id string:*/
/* 288 Feature's id string:*/
/* 289 Feature's id string:*/
/* 290 Feature's id string:*/
/* 291 Feature's id string:*/
/* 292 Feature's id string:*/
/* 293 Feature's id string:*/
/* 294 Feature's id string:*/
/* 295 Feature's id string:*/
/* 296 Feature's id string:*/
/* 297 Feature's id string:*/
/* 298 Feature's id string:*/
/* 299 Feature's id string:*/
/* 300 Feature's id string:*/
/* 301 Feature's id string:*/
/* 302 Feature's id string:*/
/* 303 Feature's id string:*/
/* 304 Feature's id string:*/
/* 305 Feature's id string:*/
/* 306 Feature's id string:*/
/* 307 Feature's id string:*/
/* 308 Feature's id string:*/
/* 309 Feature's id string:*/
/* 310 Feature's id string:*/
/* 311 Feature's id string:*/
/* 312 Feature's id string:*/
/* 313 Feature's id string:*/
/* 314 Feature's id string:*/
/* 315 Feature's id string:*/
/* 316 Feature's id string:*/
/* 317 Feature's id string:*/
/* 318 Feature's id string:*/
/* 319 Feature's id string:*/
/* 320 Feature's id string:*/
/* 321 Feature's id string:*/
/* 322 Feature's id string:*/
/* 323 Feature's id string:*/
/* 324 Feature's id string:*/
/* 325 Feature's id string:*/
/* 326 Feature's id string:*/
/* 327 Feature's id string:*/
/* 328 Feature's id string:*/
/* 329 Feature's id string:*/
/* 330 Feature's id string:*/
/* 331 Feature's id string:*/
/* 332 Feature's id string:*/
/* 333 Feature's id string:*/
/* 334 Feature's id string:*/
/* 335 Feature's id string:*/
/* 336 Feature's id string:*/
/* 337 Feature's id string:*/
/* 338 Feature's id string:*/
/* 339 Feature's id string:*/
/* 340 Feature's id string:*/
/* 341 Feature's id string:*/
/* 342 Feature's id string:*/
/* 343 Feature's id string:*/
/* 344 Feature's id string:*/
/* 345 Feature's id string:*/
/* 346 Feature's id string:*/
/* 347 Feature's id string:*/
/* 348 Feature's id string:*/
/* 349 Feature's id string:*/
/* 350 Feature's id string:*/
/* 351 Feature's id string:*/
/* 352 Feature's id string:*/
/* 353 Feature's id string:*/
/* 354 Feature's id string:*/
/* 355 Feature's id string:*/
/* 356 Feature's id string:*/
/* 357 Feature's id string:*/
/* 358 Feature's id string:*/
/* 359 Feature's id string:*/
/* 360 Feature's id string:*/
/* 361 Feature's id string:*/
/* 362 Feature's id string:*/
/* 363 Feature's id string:*/
/* 364 Feature's id string:*/
/* 365 Feature's id string:*/
/* 366 Feature's id string:*/
/* 367 Feature's id string:*/
/* 368 Feature's id string:*/
/* 369 Feature's id string:*/
/* 370 Feature's id string:*/
/* 371 Feature's id string:*/
/* 372 Feature's id string:*/
/* 373 Feature's id string:*/
/* 374 Feature's id string:*/
/* 375 Feature's id string:*/
/* 376 Feature's id string:*/
/* 377 Feature's id string:*/
/* 378 Feature's id string:*/
/* 379 Feature's id string:*/
/* 380 Feature's id string:*/
/* 381 Feature's id string:*/
/* 382 Feature's id string:*/
/* 383 Feature's id string:*/
/* 384 Feature's id string:*/
/* 385 Feature's id string:*/
/* 386 Feature's id string:*/
/* 387 Feature's id string:*/
/* 388 Feature's id string:*/
/* 389 Feature's id string:*/
/* 390 Feature's id string:*/
/* 391 Feature's id string:*/
/* 392 Feature's id string:*/
/* 393 Feature's id string:*/
/* 394 Feature's id string:*/
/* 395 Feature's id string:*/
/* 396 Feature's id string:*/
/* 397 Feature's id string:*/
/* 398 Feature's id string:*/
/* 399 Feature's id string:*/
/* 400 Feature's id string:*/
/* 401 Feature's id string:*/
/* 402 Feature's id string:*/
/* 403 Feature's id string:*/
/* 404 Feature's id string:*/
/* 405 Feature's id string:*/
/* 406 Feature's id string:*/
/* 407 Feature's id string:*/
/* 408 Feature's id string:*/
/* 409 Feature's id string:*/
/* 410 Feature's id string:*/
/* 411 Feature's id string:*/
/* 412 Feature's id string:*/
/* 413 Feature's id string:*/
/* 414 Feature's id string:*/
/* 415 Feature's id string:*/
/* 416 Feature's id string:*/
/* 417 Feature's id string:*/
/* 418 Feature's id string:*/
/* 419 Feature's id string:*/
/* 420 Feature's id string:*/
/* 421 Feature's id string:*/
/* 422 Feature's id string:*/
/* 423 Feature's id string:*/
/* 424 Feature's id string:*/
/* 425 Feature's id string:*/
/* 426 Feature's id string:*/
/* 427 Feature's id string:*/
/* 428 Feature's id string:*/
/* 429 Feature's id string:*/
/* 430 Feature's id string:*/
/* 431 Feature's id string:*/
/* 432 Feature's id string:*/
/* 433 Feature's id string:*/
/* 434 Feature's id string:*/
/* 435 Feature's id string:*/
/* 436 Feature's id string:*/
/* 437 Feature's id string:*/
/* 438 Feature's id string:*/
/* 439 Feature's id string:*/
/* 440 Feature's id string:*/
/* 441 Feature's id string:*/
/* 442 Feature's id string:*/
/* 443 Feature's id string:*/
/* 444 Feature's id string:*/
/* 445 Feature's id string:*/
/* 446 Feature's id string:*/
/* 447 Feature's id string:*/
/* 448 Feature's id string:*/
/* 449 Feature's id string:*/
/* 450 Feature's id string:*/
/* 451 Feature's id string:*/
/* 452 Feature's id string:*/
/* 453 Feature's id string:*/
/* 454 Feature's id string:*/
/* 455 Feature's id string:*/
/* 456 Feature's id string:*/
/* 457 Feature's id string:*/
/* 458 Feature's id string:*/
/* 459 Feature's id string:*/
/* 460 Feature's id string:*/
/* 461 Feature's id string:*/
/* 462 Feature's id string:*/
/* 463 Feature's id string:*/
/* 464 Feature's id string:*/
/* 465 Feature's id string:*/
/* 466 Feature's id string:*/
/* 467 Feature's id string:*/
/* 468 Feature's id string:*/
/* 469 Feature's id string:*/
/* 470 Feature's id string:*/
/* 471 Feature's id string:*/
/* 472 Feature's id string:*/
/* 473 Feature's id string:*/
/* 474 Feature's id string:*/
/* 475 Feature's id string:*/
/* 476 Feature's id string:*/
/* 477 Feature's id string:*/
/* 478 Feature's id string:*/
/* 479 Feature's id string:*/
/* 480 Feature's id string:*/
/* 481 Feature's id string:*/
/* 482 Feature's id string:*/
/* 483 Feature's id string:*/
/* 484 Feature's id string:*/
/* 485 Feature's id string:*/
/* 486 Feature's id string:*/
/* 487 Feature's id string:*/
/* 488 Feature's id string:*/
/* 489 Feature's id string:*/
/* 490 Feature's id string:*/
/* 491 Feature's id string:*/
/* 492 Feature's id string:*/
/* 493 Feature's id string:*/
/* 494 Feature's id string:*/
/* 495 Feature's id string:*/
/* 496 Feature's id string:*/
/* 497 Feature's id string:*/
/* 498 Feature's id string:*/
/* 499 Feature's id string:*/
/* 500 Feature's id string:*/
/* 501 Feature's id string:*/
/* 502 Feature's id string:*/
/* 503 Feature's id string:*/
/* 504 Feature's id string:*/
/* 505 Feature's id string:*/
/* 506 Feature's id string:*/
/* 507 Feature's id string:*/
/* 508 Feature's id string:*/
/* 509 Feature's id string:*/
/* 510 Feature's id string:*/
/* 511 Feature's id string:*/
/* 512 Feature's id string:*/
/* 513 Feature's id string:*/
/* 514 Feature's id string:*/
/* 515 Feature's id string:*/
/* 516 Feature's id string:*/
/* 517 Feature's id string:*/
/* 518 Feature's id string:*/
/* 519 Feature's id string:*/
/* 520 Feature's id string:*/
/* 521 Feature's id string:*/
/* 522 Feature's id string:*/
/* 523 Feature's id string:*/
/* 524 Feature's id string:*/
/* 525 Feature's id string:*/
/* 526 Feature's id string:*/
/* 527 Feature's id string:*/
/* 528 Feature's id string:*/
/* 529 Feature's id string:*/
/* 530 Feature's id string:*/
/* 531 Feature's id string:*/
/* 532 Feature's id string:*/
/* 533 Feature's id string:*/
/* 534 Feature's id string:*/
/* 535 Feature's id string:*/
/* 536 Feature's id string:*/
/* 537 Feature's id string:*/
/* 538 Feature's id string:*/
/* 539 Feature's id string:*/
/* 540 Feature's id string:*/
/* 541 Feature's id string:*/
/* 542 Feature's id string:*/
/* 543 Feature's id string:*/
/* 544 Feature's id string:*/
/* 545 Feature's id string:*/
/* 546 Feature's id string:*/
/* 547 Feature's id string:*/
/* 548 Feature's id string:*/
/* 549 Feature's id string:*/
/* 550 Feature's id string:*/
/* 551 Feature's id string:*/
/* 552 Feature's id string:*/
/* 553 Feature's id string:*/
/* 554 Feature's id string:*/
/* 555 Feature's id string:*/
/* 556 Feature's id string:*/
/* 557 Feature's id string:*/
/* 558 Feature's id string:*/
/* 559 Feature's id string:*/
/* 560 Feature's id string:*/
/* 561 Feature's id string:*/
/* 562 Feature's id string:*/
/* 563 Feature's id string:*/
/* 564 Feature's id string:*/
/* 565 Feature's id string:*/
/* 566 Feature's id string:*/
/* 567 Feature's id string:*/
/* 568 Feature's id string:*/
/* 569 Feature's id string:*/
/* 570 Feature's id string:*/
/* 571 Feature's id string:*/
/* 572 Feature's id string:*/
/* 573 Feature's id string:*/
/* 574 Feature's id string:*/
/* 575 Feature's id string:*/
/* 576 Feature's id string:*/
/* 577 Feature's id string:*/
/* 578 Feature's id string:*/
/* 579 Feature's id string:*/
/* 580 Feature's id string:*/
/* 581 Feature's id string:*/
/* 582 Feature's id string:*/
/* 583 Feature's id string:*/
/* 584 Feature's id string:*/
/* 585 Feature's id string:*/
/* 586 Feature's id string:*/
/* 587 Feature's id string:*/
/* 588 Feature's id string:*/
/* 589 Feature's id string:*/
/* 590 Feature's id string:*/
/* 591 Feature's id string:*/
/* 592 Feature's id string:*/
/* 593 Feature's id string:*/
/* 594 Feature's id string:*/
/* 595 Feature's id string:*/
/* 596 Feature's id string:*/
/* 597 Feature's id string:*/
/* 598 Feature's id string:*/
/* 599 Feature's id string:*/
/* 600 Feature's id string:*/
/* 601 Feature's id string:*/
/* 602 Feature's id string:*/
/* 603 Feature's id string:*/
/* 604 Feature's id string:*/
/* 605 Feature's id string:*/
/* 606 Feature's id string:*/
/* 607 Feature's id string:*/
/* 608 Feature's id string:*/
/* 609 Feature's id string:*/
/* 610 Feature's id string:*/
/* 611 Feature's id string:*/
/* 612 Feature's id string:*/
/* 613 Feature's id string:*/
/* 614 Feature's id string:*/
/* 615 Feature's id string:*/
/* 616 Feature's id string:*/
/* 617 Feature's id string:*/
/* 618 Feature's id string:*/
/* 619 Feature's id string:*/
/* 620 Feature's id string:*/
/* 621 Feature's id string:*/
/* 622 Feature's id string:*/
/* 623 Feature's id string:*/
/* 624 Feature's id string:*/
/* 625 Feature's id string:*/
/* 626 Feature's id string:*/
/* 627 Feature's id string:*/
/* 628 Feature's id string:*/
/* 629 Feature's id string:*/
/* 630 Feature's id string:*/
/* 631 Feature's id string:*/
/* 632 Feature's id string:*/
/* 633 Feature's id string:*/
/* 634 Feature's id string:*/
/* 635 Feature's id string:*/
/* 636 Feature's id string:*/
/* 637 Feature's id string:*/
/* 638 Feature's id string:*/
/* 639 Feature's id string:*/
/* 640 Feature's id string:*/
/* 641 Feature's id string:*/
/* 642 Feature's id string:*/
/* 643 Feature's id string:*/
/* 644 Feature's id string:*/
/* 645 Feature's id string:*/
/* 646 Feature's id string:*/
/* 647 Feature's id string:*/
/* 648 Feature's id string:*/
/* 649 Feature's id string:*/
/* 650 Feature's id string:*/
/* 651 Feature's id string:*/
/* 652 Feature's id string:*/
/* 653 Feature's id string:*/
/* 654 Feature's id string:*/
/* 655 Feature's id string:*/
/* 656 Feature's id string:*/
/* 657 Feature's id string:*/
/* 658 Feature's id string:*/
/* 659 Feature's id string:*/
/* 660 Feature's id string:*/
/* 661 Feature's id string:*/
/* 662 Feature's id string:*/
/* 663 Feature's id string:*/
/* 664 Feature's id string:*/
/* 665 Feature's id string:*/
/* 666 Feature's id string:*/
/* 667 Feature's id string:*/
/* 668 Feature's id string:*/
/* 669 Feature's id string:*/
/* 670 Feature's id string:*/
/* 671 Feature's id string:*/
/* 672 Feature's id string:*/
/* 673 Feature's id string:*/
/* 674 Feature's id string:*/
/* 675 Feature's id string:*/
/* 676 Feature's id string:*/
/* 677 Feature's id string:*/
/* 678 Feature's id string:*/
/* 679 Feature's id string:*/
/* 680 Feature's id string:*/
/* 681 Feature's id string:*/
/* 682 Feature's id string:*/
/* 683 Feature's id string:*/
/* 684 Feature's id string:*/
/* 685 Feature's id string:*/
/* 686 Feature's id string:*/
/* 687 Feature's id string:*/
/* 688 Feature's id string:*/
/* 689 Feature's id string:*/
/* 690 Feature's id string:*/
/* 691 Feature's id string:*/
/* 692 Feature's id string:*/
/* 693 Feature's id string:*/
/* 694 Feature's id string:*/
/* 695 Feature's id string:*/
/* 696 Feature's id string:*/
/* 697 Feature's id string:*/
/* 698 Feature's id string:*/
/* 699 Feature's id string:*/
/* 700 Feature's id string:*/
/* 701 Feature's id string:*/
/* 702 Feature's id string:*/
/* 703 Feature's id string:*/
/* 704 Feature's id string:*/
/* 705 Feature's id string:*/
/* 706 Feature's id string:*/
/* 707 Feature's id string:*/
/* 708 Feature's id string:*/
/* 709 Feature's id string:*/
/* 710 Feature's id string:*/
/* 711 Feature's id string:*/
/* 712 Feature's id string:*/
/* 713 Feature's id string:*/
/* 714 Feature's id string:*/
/* 715 Feature's id string:*/
/* 716 Feature's id string:*/
/* 717 Feature's id string:*/
/* 718 Feature's id string:*/
/* 719 Feature's id string:*/
/* 720 Feature's id string:*/
/* 721 Feature's id string:*/
/* 722 Feature's id string:*/
/* 723 Feature's id string:*/
/* 724 Feature's id string:*/
/* 725 Feature's id string:*/
/* 726 Feature's id string:*/
/* 727 Feature's id string:*/
/* 728 Feature's id string:*/
/* 729 Feature's id string:*/
/* 730 Feature's id string:*/
/* 731 Feature's id string:*/
/* 732 Feature's id string:*/
/* 733 Feature's id string:*/
/* 734 Feature's id string:*/
/* 735 Feature's id string:*/
/* 736 Feature's id string:*/
/* 737 Feature's id string:*/
/* 738 Feature's id string:*/
/* 739 Feature's id string:*/
/* 740 Feature's id string:*/
/* 741 Feature's id string:*/
/* 742 Feature's id string:*/
/* 743 Feature's id string:*/
/* 744 Feature's id string:*/
/* 745 Feature's id string:*/
/* 746 Feature's id string:*/
/* 747 Feature's id string:*/
/* 748 Feature's id string:*/
/* 749 Feature's id string:*/
/* 750 Feature's id string:*/
/* 751 Feature's id string:*/
/* 752 Feature's id string:*/
/* 753 Feature's id string:*/
/* 754 Feature's id string:*/
/* 755 Feature's id string:*/
/* 756 Feature's id string:*/
/* 757 Feature's id string:*/
/* 758 Feature's id string:*/
/* 759 Feature's id string:*/
/* 760 Feature's id string:*/
/* 761 Feature's id string:*/
/* 762 Feature's id string:*/
/* 763 Feature's id string:*/
/* 764 Feature's id string:*/
/* 765 Feature's id string:*/
/* 766 Feature's id string:*/
/* 767 Feature's id string:*/
/* 768 Feature's id string:*/
/* 769 Feature's id string:*/
/* 770 Feature's id string:*/
/* 771 Feature's id string:*/
/* 772 Feature's id string:*/
/* 773 Feature's id string:*/
/* 774 Feature's id string:*/
/* 775 Feature's id string:*/
/* 776 Feature's id string:*/
/* 777 Feature's id string:*/
/* 778 Feature's id string:*/
/* 779 Feature's id string:*/
/* 780 Feature's id string:*/
/* 781 Feature's id string:*/
/* 782 Feature's id string:*/
/* 783 Feature's id string:*/
/* 784 Feature's id string:*/
/* 785 Feature's id string:*/
/* 786 Feature's id string:*/
/* 787 Feature's id string:*/
/* 788 Feature's id string:*/
/* 789 Feature's id string:*/
/* 790 Feature's id string:*/
/* 791 Feature's id string:*/
/* 792 Feature's id string:*/
/* 793 Feature's id string:*/
/* 794 Feature's id string:*/
/* 795 Feature's id string:*/
/* 796 Feature's id string:*/
/* 797 Feature's id string:*/
/* 798 Feature's id string:*/
/* 799 Feature's id string:*/
/* 800 Feature's id string:*/
/* 801 Feature's id string:*/
/* 802 Feature's id string:*/
/* 803 Feature's id string:*/
/* 804 Feature's id string:*/
/* 805 Feature's id string:*/
/* 806 Feature's id string:*/
/* 807 Feature's id string:*/
/* 808 Feature's id string:*/
/* 809 Feature's id string:*/
/* 810 Feature's id string:*/
/* 811 Feature's id string:*/
/* 812 Feature's id string:*/
/* 813 Feature's id string:*/
/* 814 Feature's id string:*/
/* 815 Feature's id string:*/
/* 816 Feature's id string:*/
/* 817 Feature's id string:*/
/* 818 Feature's id string:*/
/* 819 Feature's id string:*/
/* 820 Feature's id string:*/
/* 821 Feature's id string:*/
/* 822 Feature's id string:*/
/* 823 Feature's id string:*/
/* 824 Feature's id string:*/
/* 825 Feature's id string:*/
/* 826 Feature's id string:*/
/* 827 Feature's id string:*/
/* 828 Feature's id string:*/
/* 829 Feature's id string:*/
/* 830 Feature's id string:*/
/* 831 Feature's id string:*/
/* 832 Feature's id string:*/
/* 833 Feature's id string:*/
/* 834 Feature's id string:*/
/* 835 Feature's id string:*/
/* 836 Feature's id string:*/
/* 837 Feature's id string:*/
/* 838 Feature's id string:*/
/* 839 Feature's id string:*/
/* 840 Feature's id string:*/
/* 841 Feature's id string:*/
/* 842 Feature's id string:*/
/* 843 Feature's id string:*/
/* 844 Feature's id string:*/
/* 845 Feature's id string:*/
/* 846 Feature's id string:*/
/* 847 Feature's id string:*/
/* 848 Feature's id string:*/
/* 849 Feature's id string:*/
/* 850 Feature's id string:*/
/* 851 Feature's id string:*/
/* 852 Feature's id string:*/
/* 853 Feature's id string:*/
/* 854 Feature's id string:*/
/* 855 Feature's id string:*/
/* 856 Feature's id string:*/
/* 857 Feature's id string:*/
/* 858 Feature's id string:*/
/* 859 Feature's id string:*/
/* 860 Feature's id string:*/
/* 861 Feature's id string:*/
/* 862 Feature's id string:*/
/* 863 Feature's id string:*/
/* 864 Feature's id string:*/
/* 865 Feature's id string:*/
/* 866 Feature's id string:*/
/* 867 Feature's id string:*/
/* 868 Feature's id string:*/
/* 869 Feature's id string:*/
/* 870 Feature's id string:*/
/* 871 Feature's id string:*/
/* 872 Feature's id string:*/
/* 873 Feature's id string:*/
/* 874 Feature's id string:*/
/* 875 Feature's id string:*/
/* 876 Feature's id string:*/
/* 877 Feature's id string:*/
/* 878 Feature's id string:*/
/* 879 Feature's id string:*/
/* 880 Feature's id string:*/
/* 881 Feature's id string:*/
/* 882 Feature's id string:*/
/* 883 Feature's id string:*/
/* 884 Feature's id string:*/
/* 885 Feature's id string:*/
/* 886 Feature's id string:*/
/* 887 Feature's id string:*/
/* 888 Feature's id string:*/
/* 889 Feature's id string:*/
/* 890 Feature's id string:*/
/* 891 Feature's id string:*/
/* 892 Feature's id string:*/
/* 893 Feature's id string:*/
/* 894 Feature's id string:*/
/* 895 Feature's id string:*/
/* 896 Feature's id string:*/
/* 897 Feature's id string:*/
/* 898 Feature's id string:*/
/* 899 Feature's id string:*/
/* 900 Feature's id string:*/
/* 901 Feature's id string:*/
/* 902 Feature's id string:*/
/* 903 Feature's id string:*/
/* 904 Feature's id string:*/
/* 905 Feature's id string:*/
/* 906 Feature's id string:*/
/* 907 Feature's id string:*/
/* 908 Feature's id string:*/
/* 909 Feature's id string:*/
/* 910 Feature's id string:*/
/* 911 Feature's id string:*/
/* 912 Feature's id string:*/
/* 913 Feature's id string:*/
/* 914 Feature's id string:*/
/* 915 Feature's id string:*/
/* 916 Feature's id string:*/
/* 917 Feature's id string:*/
/* 918 Feature's id string:*/
/* 919 Feature's id string:*/
/* 920 Feature's id string:*/
/* 921 Feature's id string:*/
/* 922 Feature's id string:*/
/* 923 Feature's id string:*/
/* 924 Feature's id string:*/
/* 925 Feature's id string:*/
/* 926 Feature's id string:*/
/* 927 Feature's id string:*/
/* 928 Feature's id string:*/
/* 929 Feature's id string:*/
/* 930 Feature's id string:*/
/* 931 Feature's id string:*/
/* 932 Feature's id string:*/
/* 933 Feature's id string:*/
/* 934 Feature's id string:*/
/* 935 Feature's id string:*/
/* 936 Feature's id string:*/
/* 937 Feature's id string:*/
/* 938 Feature's id string:*/
/* 939 Feature's id string:*/
/* 940 Feature's id string:*/
/* 941 Feature's id string:*/
/* 942 Feature's id string:*/
/* 943 Feature's id string:*/
/* 944 Feature's id string:*/
/* 945 Feature's id string:*/
/* 946 Feature's id string:*/
/* 947 Feature's id string:*/
/* 948 Feature's id string:*/
/* 949 Feature's id string:*/
/* 950 Feature's id string:*/
/* 951 Feature's id string:*/
/* 952 Feature's id string:*/
/* 953 Feature's id string:*/
/* 954 Feature's id string:*/
/* 955 Feature's id string:*/
/* 956 Feature's id string:*/
/* 957 Feature's id string:*/
/* 958 Feature's id string:*/
/* 959 Feature's id string:*/
/* 960 Feature's id string:*/
/* 961 Feature's id string:*/
/* 962 Feature's id string:*/
/* 963 Feature's id string:*/
/* 964 Feature's id string:*/
/* 965 Feature's id string:*/
/* 966 Feature's id string:*/
/* 967 Feature's id string:*/
/* 968 Feature's id string:*/
/* 969 Feature's id string:*/
/* 970 Feature's id string:*/
/* 971 Feature's id string:*/
/* 972 Feature's id string:*/
/* 973 Feature's id string:*/
/* 974 Feature's id string:*/
/* 975 Feature's id string:*/
/* 976 Feature's id string:*/
/* 977 Feature's id string:*/
/* 978 Feature's id string:*/
/* 979 Feature's id string:*/
/* 980 Feature's id string:*/
/* 981 Feature's id string:*/
/* 982 Feature's id string:*/
/* 983 Feature's id string:*/
/* 984 Feature's id string:*/
/* 985 Feature's id string:*/
/* 986 Feature's id string:*/
/* 987 Feature's id string:*/
/* 988 Feature's id string:*/
/* 989 Feature's id string:*/
/* 990 Feature's id string:*/
/* 991 Feature's id string:*/
/* 992 Feature's id string:*/
/* 993 Feature's id string:*/
/* 994 Feature's id string:*/
/* 995 Feature's id string:*/
/* 996 Feature's id string:*/
/* 997 Feature's id string:*/
/* 998 Feature's id string:*/
/* 999 Feature's id string:*/
/*1000 Feature's id string:*/
/*1001 Feature's id string:*/
/*1002 Feature's id string:*/
/*1003 Feature's id string:*/
/*1004 Feature's id string:*/
/*1005 Feature's id string:*/
/*1006 Feature's id string:*/
/*1007 Feature's id string:*/
/*1008 Feature's id string:*/
/*1009 Feature's id string:*/
/*1010 Feature's id string:*/
/*1011 Feature's id string:*/
/*1012 Feature's id string:*/
/*1013 Feature's id string:*/
/*1014 Feature's id string:*/
/*1015 Feature's id string:*/
/*1016 Feature's id string:*/
/*1017 Feature's id string:*/
/*1018 Feature's id string:*/
/*1019 Feature's id string:*/
/*1020 Feature's id string:*/
/*1021 Feature's id string:*/
/*1022 Feature's id string:*/
/*1023 Feature's id string:*/
/*1024 Feature's id string:*/
};