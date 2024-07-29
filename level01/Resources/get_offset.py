if __name__ == "__main__":
    base_value = 0x41
    input_value = int(input("Enter an hexadecimal value:"), 16)
    print(f"Offset: {input_value - base_value}, for character: {chr(input_value)}")