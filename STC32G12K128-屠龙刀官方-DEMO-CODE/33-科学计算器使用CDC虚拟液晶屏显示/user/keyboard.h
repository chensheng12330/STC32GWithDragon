

char judge_num(uchar *s_count, uchar *gb_count, uchar num0, char *temp1)
{
	uchar i, j;
	if ((((num0 <= 0x39) && (num0 >= 0x28)) || (num0 == 2) || (num0 == 32) || (num0 == 'x') || (num0 == 'y') || (num0 == 'z') || (num0 == '^') || (num0 == '!') || (num0 == 'e') || (num0 == 'i')) && ((*s_count) < 25))
	{ //µ•∏ˆ◊÷∑˚
		if ((*gb_count) != (*s_count))
		{
			for (i = (*s_count); i > (*gb_count) - 1; i--)
				temp1[i] = temp1[i - 1];
		}

		if ((num0 != 2) && (num0 != 32))
			temp1[(*gb_count)] = num0;
		else if (num0 == 2)
			temp1[(*gb_count)] = 'p';
		else if (num0 == 32)
			temp1[(*gb_count)] = 'd';
		(*s_count)++;
		(*gb_count)++;
		num0 = 0xff;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (((*s_count) < 24) && (num0 == 9)) //¡Ω∏ˆ◊÷∑˚
	{
		if ((*gb_count) != (*s_count))
		{
			for (i = (*s_count) + 1; i > (*gb_count); i--)
				temp1[i] = temp1[i - 2];
		}
		for (j = (*gb_count); j < (*gb_count) + 2; j++)
			temp1[j] = x_2[j - (*gb_count)];
		(*s_count) = (*s_count) + 2;
		(*gb_count) = (*gb_count) + 2;
		num0 = 0xff;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (((*s_count) < 23) && ((num0 == 12) || (num0 == 27) || (num0 == 20))) // 3∏ˆ◊÷∑˚
	{
		if ((*gb_count) != (*s_count))
		{
			for (i = (*s_count) + 2; i > (*gb_count) + 1; i--)
				temp1[i] = temp1[i - 3];
		}
		for (j = (*gb_count); j < (*gb_count) + 3; j++)
			switch (num0)
			{
			case 12:
				temp1[j] = exp[j - (*gb_count)];
				break;
			case 27:
				temp1[j] = ln[j - (*gb_count)];
				break;
			case 20:
				temp1[j] = ans[j - (*gb_count)];
				break;
			}
		(*s_count) = (*s_count) + 3;
		(*gb_count) = (*gb_count) + 3;
		num0 = 0xff;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (((*s_count) < 22) && ((num0 == 15) || (num0 == 16) || (num0 == 17) || (num0 == 28) || (num0 == 13) || (num0 == 8) || (num0 == 26)))
	{ // 4∏ˆ◊÷∑˚
		if ((*gb_count) != (*s_count))
		{
			for (i = (*s_count) + 3; i > (*gb_count) + 2; i--)
				temp1[i] = temp1[i - 4];
		}
		for (j = (*gb_count); j < (*gb_count) + 4; j++)
			switch (num0)
			{
			case 15:
				temp1[j] = sin[j - (*gb_count)];
				break;
			case 16:
				temp1[j] = cos[j - (*gb_count)];
				break;
			case 17:
				temp1[j] = tan[j - (*gb_count)];
				break;
			case 28:
				temp1[j] = log[j - (*gb_count)];
				break;
			case 13:
				temp1[j] = x_10[j - (*gb_count)];
				break;
			case 8:
				temp1[j] = abs[j - (*gb_count)];
				break;
			case 26:
				temp1[j] = x_1y[j - (*gb_count)];
				break;
			}
		(*s_count) = (*s_count) + 4;
		(*gb_count) = (*gb_count) + 4;
		num0 = 0xff;
	}

	//////////////////////////////////////////////////////////////////////////////
	else if (((*s_count) < 21) && ((num0 == 29) || (num0 == 30) || (num0 == 31) || (num0 == 11)))
	{ // 5∏ˆ◊÷∑˚
		if ((*gb_count) != (*s_count))
		{
			for (i = (*s_count) + 4; i > (*gb_count) + 3; i--)
				temp1[i] = temp1[i - 5];
		}
		for (j = (*gb_count); j < (*gb_count) + 5; j++)
			switch (num0)
			{
			case 29:
				temp1[j] = asin[j - (*gb_count)];
				break;
			case 30:
				temp1[j] = acos[j - (*gb_count)];
				break;
			case 31:
				temp1[j] = atan[j - (*gb_count)];
				break;
			case 11:
				temp1[j] = x_1[j - (*gb_count)];
				break;
			}
		(*s_count) = (*s_count) + 5;
		(*gb_count) = (*gb_count) + 5;
		num0 = 0xff;
	}

	//////////////////////////////////////////////////////////////////////////
	else if (((*s_count) < 20) && (num0 == 25)) // 6∏ˆ◊÷∑˚
	{
		if ((*gb_count) != (*s_count))
		{
			for (i = (*s_count) + 5; i > (*gb_count) + 4; i--)
				temp1[i] = temp1[i - 6];
		}
		for (j = (*gb_count); j < (*gb_count) + 6; j++)
			temp1[j] = x_05[j - (*gb_count)];
		(*s_count) = (*s_count) + 6;
		(*gb_count) = (*gb_count) + 6;
		num0 = 0xff;
	}

	//////////////////////////////////////////////////////////////////////////
	else if (num0 == 14) // hyp
	{
		close_cursor();
		num0 = 0xff;
		lcd_clear();
		lcd_set_pos(0, 0);
		lcd_write_str("1.sinh");
		lcd_set_pos(1, 0);
		lcd_write_str("2.cosh");
		lcd_set_pos(2, 0);
		lcd_write_str("3.tanh");

		while (num0 == 0xff)
		{
			num0 = mode_choose();
			switch (num0)
			{
			case 1:
				num0 = HYP_SINH;
				break;
			case 2:
				num0 = HYP_COSH;
				break;
			case 3:
				num0 = HYP_TANH;
				break;
			default:
				num0 = 0xff;
			}
		}

		if ((*gb_count) != (*s_count))
		{
			for (i = (*s_count) + 4; i > (*gb_count) + 3; i--)
				temp1[i] = temp1[i - 5];
		}
		for (j = (*gb_count); j < (*gb_count) + 5; j++)
			switch (num0)
			{
			case HYP_SINH:
				temp1[j] = sinh[j - (*gb_count)];
				break;
			case HYP_COSH:
				temp1[j] = cosh[j - (*gb_count)];
				break;
			case HYP_TANH:
				temp1[j] = tanh[j - (*gb_count)];
				break;
			}
		(*s_count) = (*s_count) + 5;
		(*gb_count) = (*gb_count) + 5;
		num0 = 0xff;
		lcd_clear();
	}

	//////////////////////////////////////////////////////////////////////////
	/// copy
	else if (num0 == 21)
	{
		num0 = 0xff;
		//				return INPUT_COPY;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (num0 == 18) /// delete
	{
		num0 = 0xff;
		if (((*gb_count) == 25) && ((*s_count) == 25))
		{
			temp1[24] = ' ';
			(*s_count) = 24;
			(*gb_count) = 24;
		}
		else if ((*gb_count) > 0)
		{
			for (i = (*gb_count) - 1; i < (*s_count); i++)
				temp1[i] = temp1[i + 1];
			(*s_count) = (*s_count) - 1;
			(*gb_count) = (*gb_count) - 1;
			if ((*s_count) < 0)
				(*s_count) = 0;
			if ((*gb_count) < 0)
				(*gb_count) = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	else if (num0 == 7) //”““∆
	{
		if ((*gb_count) != (*s_count))
			((*gb_count))++;
		num0 = 0xff;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (num0 == 5) //◊Û“∆
	{
		if ((*gb_count > 0))
			(*gb_count)--;
		num0 = 0xff;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (num0 == 3) //…œ“∆
	{
		return INPUT_UP;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (num0 == 6) //œ¬“∆
	{
		return INPUT_DOWN;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (num0 == 19) //«Â¡„ac
	{
		num0 = 0xff;

		(*gb_count) = 0;
		(*s_count) = 0;
		for (i = 0; i < 25; i++)
			temp1[i] = ' ';
		return INPUT_AC;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (num0 == '=') //Ω· ¯ ‰»Î
	{
		temp1[(*gb_count)] = '\0';
		num0 = 0xff;
		(*gb_count) = 0;
		(*s_count) = 0;

		return INPUT_EQ;
	}
	return INPUT_NORMAL;
}