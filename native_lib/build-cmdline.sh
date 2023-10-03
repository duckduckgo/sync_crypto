g++ -I/opt/homebrew/include/ -L/opt/homebrew/lib/ test_user_setup.c DDGSyncCrypto.c -lsodium -o test_user_setup

# Smoke test
output=$(./test_user_setup aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAAA= b c)
primaryKey=$(echo $output | jq -r '.primaryKey')
secretKey=$(echo $output | jq -r '.secretKey')
passwordHash=$(echo $output | jq -r .passwordHash)

[[ $primaryKey == jJwQe7UZ2awc9PTXo93PKPoYlcdQ0ajOvQzbIzUke9c= ]] ||
	exit 1
[[ $secretKey == aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAAA= ]] ||
	exit 1
[[ $passwordHash == FWb34Cqw6oSOZUjQuU9IwMMEH9UifUBdge3Ne/YHfTg= ]] ||
	exit 1

exit 0
