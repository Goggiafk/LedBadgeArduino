if (pin3clicked == false)
        {
            savedBrightness = matrix.getBrightness();
            pin3clicked = true;
            pin3canceled = false;
            for (size_t i = savedBrightness; i > 0; i -= 1)
            {
                matrix.setBrightness(i);
                delay(3);
                matrix.show();
            }
            matrix.setBrightness(1);
        }
        else
        {
            pin3clicked = false;
            pin3canceled = false;
            for (size_t i = 1; i < savedBrightness; i += 1)
            {
                matrix.setBrightness(i);
                delay(3);
                matrix.show();
            }
            matrix.setBrightness(savedBrightness);
            pin3canceled = true;
        }